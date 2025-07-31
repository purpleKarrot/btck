// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chain.hpp"

#include <btck/btck.h>

#include <cstddef>
#include <span>

#include "block.hpp"
#include "chain.h"
#include "node/blockstorage.h"
#include "span.h"
#include "uint256.h"

//   // poor man's cpp support for named arguments
//   struct KwArgs
//   {
//     KwArgs();
//     auto chain_type(ChainType arg) && -> KwArgs;
//     auto validation(Validation arg) && -> KwArgs;

//     template <typename T>
//     auto notifications(KernelNotifications<T> arg) && -> KwArgs;

//     auto SetWorkerThreads(int worker_threads) && -> KwArgs;
//     auto SetWipeDbs(bool wipe_block_tree, bool wipe_chainstate) && -> KwArgs;
//     auto SetBlockTreeDbInMemory(bool block_tree_db_in_memory) && -> KwArgs;
//     auto SetChainstateDbInMemory(bool chainstate_db_in_memory) && -> KwArgs;
//   };

//   Chain(std::string_view data_dir, std::string_view blocks_dir, KwArgs kwargs = {});
// BTCK_API struct BtcK_Chain* BtcK_Chain_New();

extern "C" {

auto BtcK_Chain_Retain(BtcK_Chain* self) -> BtcK_Chain*
{
  return self->Retain();
}

void BtcK_Chain_Release(BtcK_Chain* self)
{
  self->Release();
}

auto BtcK_Chain_NumBlocks(BtcK_Chain const* self) -> std::size_t
{
  return self->chainstate_manager.ActiveChain().Height();
}

auto BtcK_Chain_GetBlock(BtcK_Chain const* self, std::size_t idx) -> BtcK_Block*
{
  CBlockIndex* bi = self->chainstate_manager.ActiveChain()[int(idx)];
  return BtcK_Block::New(*bi, self->chainstate_manager.m_blockman);
}

auto BtcK_Chain_FindBlock(
  BtcK_Chain const* self, BtcK_BlockHash const* block_hash) -> std::ptrdiff_t
{
  auto const hash = uint256{std::span{block_hash->data}};
  auto const* bi = self->chainstate_manager.m_blockman.LookupBlockIndex(hash);
  return (bi != nullptr && self->chainstate_manager.ActiveChain().Contains(bi))
    ? bi->nHeight
    : -1;
}

}  // extern "C"
