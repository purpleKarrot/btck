// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "transaction.hpp"

#include <cstddef>
#include <memory>
#include <span>
#include <utility>

#include <serialize.h>
#include <streams.h>

#include <btck/btck.h>
#include <primitives/transaction.h>

#include "span.h"
#include "transaction_output.hpp"
#include "util/as_bytes.hpp"
#include "util/error.hpp"

namespace {

auto make(std::span<std::byte const> raw)
{
  auto stream = DataStream{raw};
  return std::make_shared<CTransaction>(deserialize, TX_WITH_WITNESS, stream);
}

}  // namespace

BtcK_Transaction::BtcK_Transaction(CTransactionRef arg)
  : transaction{std::move(arg)}
  , string{transaction->ToString()}
{
  // TODO: do we even need this serialzed?
}

BtcK_Transaction::BtcK_Transaction(std::span<std::byte const> raw)
  : transaction{make(raw)}
  , serialized(raw.begin(), raw.end())
  , string(transaction->ToString())
{}

extern "C" {

auto BtcK_Transaction_New(
  void const* raw, std::size_t len, struct BtcK_Error** err
) -> BtcK_Transaction*
{
  return util::WrapFn(err, [=] {
    return new BtcK_Transaction{
      std::span{reinterpret_cast<std::byte const*>(raw), len}
    };
  });
}

auto BtcK_Transaction_Retain(BtcK_Transaction* self) -> BtcK_Transaction*
{
  return self->Retain();
}

void BtcK_Transaction_Release(BtcK_Transaction* self)
{
  self->Release();
}

auto BtcK_Transaction_NumOutputs(BtcK_Transaction const* self) -> std::size_t
{
  return self->transaction->vout.size();
}

auto BtcK_Transaction_GetOutput(BtcK_Transaction const* self, std::size_t idx)
  -> BtcK_TransactionOutput*
{
  return BtcK_TransactionOutput::New(self->transaction->vout[idx]);
}

auto BtcK_Transaction_AsBytes(BtcK_Transaction const* self, std::size_t* len)
  -> void const*
{
  return util::AsBytes(self->serialized, len);
}

auto BtcK_Transaction_ToString(BtcK_Transaction const* self, std::size_t* len)
  -> char const*
{
  if (len != nullptr) {
    *len = self->string.size();
  }
  return self->string.c_str();
}

}  // extern "C"
