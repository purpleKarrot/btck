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

#include <primitives/transaction.h>

#include "transaction_output.hpp"
#include "util/as_bytes.hpp"

namespace {

auto make(std::span<std::byte const> raw)
{
  auto stream = DataStream{raw};
  return std::make_shared<CTransaction>(deserialize, TX_WITH_WITNESS, stream);
}

}  // namespace

btck_Transaction::btck_Transaction(CTransactionRef arg)
  : transaction{std::move(arg)}
  , string{transaction->ToString()}
{
  // TODO: do we even need this serialzed?
}

btck_Transaction::btck_Transaction(std::span<std::byte const> raw)
  : transaction{make(raw)}
  , serialized(raw.begin(), raw.end())
  , string(transaction->ToString())
{}

extern "C" {

auto btck_Transaction_New(void const* raw, size_t len) -> btck_Transaction*
{
  return btck_Transaction::New(std::span{reinterpret_cast<std::byte const*>(raw), len});
}

auto btck_Transaction_Retain(btck_Transaction* self) -> btck_Transaction*
{
  return self->Retain();
}

void btck_Transaction_Release(btck_Transaction* self)
{
  self->Release();
}

auto btck_Transaction_GetSize(btck_Transaction const* self) -> std::size_t
{
  return self->transaction->vout.size();
}

auto btck_Transaction_At(btck_Transaction const* self, size_t idx) -> btck_TransactionOutput*
{
  return btck_TransactionOutput::New(self->transaction->vout[idx]);
}

auto btck_Transaction_AsBytes(btck_Transaction const* self, size_t* len) -> void const*
{
  return util::AsBytes(self->serialized, len);
}

auto btck_Transaction_ToString(struct btck_Transaction const* self, size_t* len) -> char const*
{
  if (len != nullptr) {
    *len = self->string.size();
  }
  return self->string.c_str();
}

}  // extern "C"
