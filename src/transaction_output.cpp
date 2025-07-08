// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "transaction_output.hpp"

#include <cstdint>

#include <btck/btck.h>

#include "primitives/transaction.h"
#include "script_pubkey.hpp"

extern "C" {

auto BtcK_TransactionOutput_New(
  int64_t amount, BtcK_ScriptPubkey const* script_pubkey
) -> BtcK_TransactionOutput*
{
  return BtcK_TransactionOutput::New(amount, script_pubkey->script);
}

auto BtcK_TransactionOutput_Retain(BtcK_TransactionOutput* self)
  -> BtcK_TransactionOutput*
{
  return self->Retain();
}

void BtcK_TransactionOutput_Release(BtcK_TransactionOutput* self)
{
  self->Release();
}

auto BtcK_TransactionOutput_GetAmount(BtcK_TransactionOutput const* self)
  -> std::int64_t
{
  return self->tx_out.nValue;
}

auto BtcK_TransactionOutput_GetScriptPubkey(BtcK_TransactionOutput const* self)
  -> BtcK_ScriptPubkey*
{
  return BtcK_ScriptPubkey::New(self->tx_out.scriptPubKey);
}

}  // extern "C"
