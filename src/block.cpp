// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "block.hpp"

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <vector>

#include <streams.h>

#include <btck/btck.h>
#include <primitives/transaction.h>

#include "node/blockstorage.h"
#include "primitives/block.h"
#include "serialize.h"
#include "span.h"
#include "transaction.hpp"
#include "uint256.h"
#include "util/error.hpp"

BtcK_Block::BtcK_Block(std::span<std::byte const> raw)
{
  auto stream = DataStream{raw};
  stream >> TX_WITH_WITNESS(block);
}

BtcK_Block::BtcK_Block(CBlockIndex const& bi, node::BlockManager const& bm)
{
  if (!bm.ReadBlock(this->block, bi)) {
    throw std::runtime_error("Failed to read block.");
  }
}

extern "C" {

auto BtcK_Block_New(void const* raw, std::size_t len, struct BtcK_Error** err)
  -> BtcK_Block*
{
  return util::WrapFn(err, [=] {
    return new BtcK_Block{
      std::span{reinterpret_cast<std::byte const*>(raw), len}
    };
  });
}

auto BtcK_Block_Retain(BtcK_Block* self) -> BtcK_Block*
{
  return self->Retain();
}

void BtcK_Block_Release(BtcK_Block* self)
{
  self->Release();
}

void BtcK_Block_GetHash(BtcK_Block const* self, BtcK_BlockHash* out)
{
  auto const hash = self->block.GetHash();
  BtcK_BlockHash_Init(out, hash.data(), decltype(hash)::size());
}

auto BtcK_Block_GetSize(BtcK_Block const* self) -> std::size_t
{
  return self->block.vtx.size();
}

auto BtcK_Block_At(BtcK_Block const* self, std::size_t idx) -> BtcK_Transaction*
{
  return BtcK_Transaction::New(self->block.vtx[idx]);
}

auto BtcK_Block_AsBytes(BtcK_Block const* /*self*/, std::size_t* /*len*/)
  -> void const*
{
  return nullptr;
}

void BtcK_BlockHash_Init(
  struct BtcK_BlockHash* self, void const* raw, std::size_t len
)
{
  assert(raw != nullptr);
  assert(len == BtcK_BlockHash_SIZE);
  std::copy_n(
    reinterpret_cast<std::uint8_t const*>(raw), sizeof(self->data), self->data
  );
}

}  // extern "C"
