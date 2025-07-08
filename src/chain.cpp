// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "chain.hpp"

#include <cstddef>

#include <btck/btck.h>

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
// BTCK_API struct btck_Chain* btck_Chain_New();

extern "C" {

auto btck_Chain_Retain(btck_Chain* self) -> btck_Chain*
{
  return self->Retain();
}

void btck_Chain_Release(btck_Chain* self)
{
  self->Release();
}

auto btck_Chain_GetSize(btck_Chain const* /*self*/) -> std::size_t
{
  return 0;
}

auto btck_Chain_At(btck_Chain const* /*self*/, size_t /*idx*/) -> btck_Block*
{
  return nullptr;
}

auto btck_Chain_Find(btck_Chain const* /*self*/, btck_BlockHash const* /*block_hash*/) -> std::ptrdiff_t
{
  return 0;
}

}  // extern "C"
