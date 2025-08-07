// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <btck/btck.h>  // IWYU pragma: associated

#include <cstdint>
#include <string>
#include <utility>

#include "primitives/transaction.h"
#include "util/api.hpp"
#include "util/error.hpp"

struct BtcK_Error;

extern "C" {

auto BtcK_TransactionOutput_New(
  int64_t amount, BtcK_ScriptPubkey const* script_pubkey, BtcK_Error** err)
  -> BtcK_TransactionOutput*
{
  return util::WrapFn(err, [amount, script_pubkey] {
    return api::create<CTxOut>(amount, api::get(script_pubkey));
  });
}

auto BtcK_TransactionOutput_Copy(
  BtcK_TransactionOutput const* self, BtcK_Error** err)
  -> BtcK_TransactionOutput*
{
  return api::copy(self, err);
}

void BtcK_TransactionOutput_Free(BtcK_TransactionOutput* self)
{
  api::free(self);
}

auto BtcK_TransactionOutput_GetAmount(BtcK_TransactionOutput const* self)
  -> std::int64_t
{
  return api::get(self).nValue;
}

auto BtcK_TransactionOutput_GetScriptPubkey(BtcK_TransactionOutput const* self)
  -> BtcK_ScriptPubkey const*
{
  return api::ref(api::get(self).scriptPubKey);
}

auto BtcK_TransactionOutput_ToString(
  BtcK_TransactionOutput const* self, char* buf, size_t len) -> int
{
  auto const str = api::get(self).ToString();
  str.copy(buf, len);
  return static_cast<int>(str.size());
}

}  // extern "C"
