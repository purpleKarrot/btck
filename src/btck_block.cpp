// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <btck/btck.h>  // IWYU pragma: associated
#include <primitives/transaction.h>

#include <streams.h>

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include "primitives/block.h"
#include "serialize.h"
#include "span.h"
#include "uint256.h"
#include "util/api.hpp"
#include "util/error.hpp"
#include "util/writer_stream.hpp"

extern "C" {

auto BtcK_Block_New(void const* raw, std::size_t len, struct BtcK_Error** err)
  -> BtcK_Block*
{
  return util::WrapFn(err, [raw, len] {
    auto const data = std::span{reinterpret_cast<std::byte const*>(raw), len};
    auto block = CBlock{};
    auto stream = DataStream{data};
    stream >> TX_WITH_WITNESS(block);
    return api::create<CBlock>(std::move(block));
  });
}

auto BtcK_Block_Copy(BtcK_Block const* self, struct BtcK_Error** err)
  -> BtcK_Block*
{
  return api::copy(self, err);
}

void BtcK_Block_Free(BtcK_Block* self)
{
  api::free(self);
}

void BtcK_Block_GetHash(BtcK_Block const* self, BtcK_BlockHash* out)
{
  auto const hash = api::get(self).GetHash();
  BtcK_BlockHash_Init(out, hash.data(), decltype(hash)::size());
}

auto BtcK_Block_CountTransactions(BtcK_Block const* self) -> std::size_t
{
  return api::get(self).vtx.size();
}

auto BtcK_Block_GetTransaction(BtcK_Block const* self, std::size_t idx)
  -> BtcK_Transaction const*
{
  return api::ref(api::get(self).vtx[idx]);
}

auto BtcK_Block_ToBytes(
  BtcK_Block const* self, BtcK_WriteBytes write, void* userdata) -> int
{
  try {
    auto stream = util::WriterStream{write, userdata};
    stream << TX_WITH_WITNESS(api::get(self));
    return 0;
  }
  catch (...) {
    return -1;
  }
}

void BtcK_BlockHash_Init(
  struct BtcK_BlockHash* self, void const* raw, std::size_t len)
{
  assert(raw != nullptr);
  assert(len == BtcK_BlockHash_SIZE);
  std::copy_n(
    reinterpret_cast<std::uint8_t const*>(raw), sizeof(self->data), self->data);
}

auto BtcK_Block_ToString(BtcK_Block const* self, char* buf, size_t len) -> int
{
  auto const str = api::get(self).ToString();
  str.copy(buf, len);
  return static_cast<int>(str.size());
}

}  // extern "C"
