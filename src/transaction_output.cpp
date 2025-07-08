// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "transaction_output.hpp"

#include <cstdint>

#include "script_pubkey.hpp"

extern "C" {

auto btck_TransactionOutput_New(int64_t amount, btck_ScriptPubkey const* script_pubkey) -> btck_TransactionOutput*
{
  return btck_TransactionOutput::New(amount, script_pubkey->script);
}

auto btck_TransactionOutput_Retain(btck_TransactionOutput* self) -> btck_TransactionOutput*
{
  return self->Retain();
}

void btck_TransactionOutput_Release(struct btck_TransactionOutput* self)
{
  self->Release();
}

auto btck_TransactionOutput_GetAmount(btck_TransactionOutput const* self) -> std::int64_t
{
  return self->tx_out.nValue;
}

auto btck_TransactionOutput_GetScriptPubkey(btck_TransactionOutput const* self) -> btck_ScriptPubkey*
{
  return btck_ScriptPubkey::New(self->tx_out.scriptPubKey);
}

}  // extern "C"
