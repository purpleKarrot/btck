// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chain.hpp"

#include <cstddef>

#include <btck/btck.h>

struct BtcK_Block;

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

auto BtcK_Chain_GetSize(BtcK_Chain const* /*self*/) -> std::size_t
{
  return 0;
}

auto BtcK_Chain_At(BtcK_Chain const* /*self*/, std::size_t /*idx*/)
  -> BtcK_Block*
{
  return nullptr;
}

auto BtcK_Chain_Find(
  BtcK_Chain const* /*self*/, BtcK_BlockHash const* /*block_hash*/
) -> std::ptrdiff_t
{
  return 0;
}

}  // extern "C"
