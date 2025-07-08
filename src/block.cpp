// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "block.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <span>

#include <streams.h>

#include <btck/btck.h>
#include <primitives/transaction.h>

#include "transaction.hpp"

btck_Block::btck_Block(std::span<std::byte const> raw)
{
  auto stream = DataStream{raw};
  stream >> TX_WITH_WITNESS(block);
}

extern "C" {

auto btck_Block_New(void const* raw, size_t len) -> btck_Block*
{
  return btck_Block::New(std::span{reinterpret_cast<std::byte const*>(raw), len});
}

auto btck_Block_Retain(btck_Block* self) -> btck_Block*
{
  return self->Retain();
}

void btck_Block_Release(btck_Block* self)
{
  self->Release();
}

void btck_Block_GetHash(btck_Block const* self, btck_BlockHash* out)
{
  auto const hash = self->block.GetHash();
  btck_BlockHash_Init(out, hash.data(), decltype(hash)::size());
}

auto btck_Block_GetSize(btck_Block const* self) -> std::size_t
{
  return self->block.vtx.size();
}

auto btck_Block_At(btck_Block const* self, std::size_t idx) -> btck_Transaction*
{
  return btck_Transaction::New(self->block.vtx[idx]);
}

auto btck_Block_AsBytes(btck_Block const* /*self*/, std::size_t* /*len*/) -> void const*
{
  return nullptr;
}

void btck_BlockHash_Init(struct btck_BlockHash* self, void const* raw, std::size_t len)
{
  assert(raw != nullptr);
  assert(len == btck_BlockHash_SIZE);
  std::copy_n(reinterpret_cast<std::uint8_t const*>(raw), sizeof(self->data), self->data);
}

}  // extern "C"
