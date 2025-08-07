// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <btck/btck.h>  // IWYU pragma: associated
#include <primitives/transaction.h>

#include <serialize.h>
#include <streams.h>

#include <cstddef>
#include <memory>
#include <span>
#include <string>
#include <utility>
#include <vector>

#include "span.h"
#include "util/api.hpp"
#include "util/error.hpp"
#include "util/writer_stream.hpp"

extern "C" {

auto BtcK_Transaction_New(
  void const* raw, std::size_t len, struct BtcK_Error** err)
  -> BtcK_Transaction*
{
  return util::WrapFn(err, [raw, len] {
    auto const bytes = std::span{reinterpret_cast<std::byte const*>(raw), len};
    auto stream = DataStream{bytes};
    auto tx =
      std::make_shared<CTransaction>(deserialize, TX_WITH_WITNESS, stream);
    return api::create<CTransactionRef>(std::move(tx));
  });
}

auto BtcK_Transaction_Copy(
  BtcK_Transaction const* self, struct BtcK_Error** err) -> BtcK_Transaction*
{
  return api::copy(self, err);
}

void BtcK_Transaction_Free(BtcK_Transaction* self)
{
  api::free(self);
}

auto BtcK_Transaction_CountOutputs(BtcK_Transaction const* self) -> std::size_t
{
  return api::get(self)->vout.size();
}

auto BtcK_Transaction_GetOutput(BtcK_Transaction const* self, std::size_t idx)
  -> BtcK_TransactionOutput const*
{
  return api::ref(api::get(self)->vout[idx]);
}

auto BtcK_Transaction_ToBytes(
  BtcK_Transaction const* self, BtcK_WriteBytes write, void* userdata) -> int
{
  try {
    auto stream = util::WriterStream{write, userdata};
    SerializeTransaction(*api::get(self), stream, TX_WITH_WITNESS);
    return 0;
  }
  catch (...) {
    return -1;
  }
}

auto BtcK_Transaction_ToString(
  BtcK_Transaction const* self, char* buf, size_t len) -> int
{
  auto const str = api::get(self)->ToString();
  str.copy(buf, len);
  return static_cast<int>(str.size());
}

}  // extern "C"
