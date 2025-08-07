// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chain.hpp"

#include <btck/btck.h>  // IWYU pragma: associated

#include <cstddef>
#include <span>
#include <stdexcept>
#include <utility>

#include "chain.h"
#include "node/blockstorage.h"
#include "primitives/block.h"
#include "span.h"
#include "uint256.h"
#include "util/api.hpp"
#include "util/error.hpp"

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

void BtcK_Chain_Free(BtcK_Chain* self)
{
  delete self;
}

auto BtcK_Chain_CountBlocks(BtcK_Chain const* self) -> std::size_t
{
  return self->chainstate_manager.ActiveChain().Height();
}

auto BtcK_Chain_GetBlock(
  BtcK_Chain const* self, std::size_t idx, struct BtcK_Error** err)
  -> BtcK_Block*
{
  return util::WrapFn(err, [self, idx] {
    CBlockIndex* bi = self->chainstate_manager.ActiveChain()[int(idx)];
    node::BlockManager const& bm = self->chainstate_manager.m_blockman;

    auto block = CBlock{};
    if (!bm.ReadBlock(block, *bi)) {
      throw std::runtime_error("Failed to read block.");
    }

    return api::create<CBlock>(std::move(block));
  });
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
