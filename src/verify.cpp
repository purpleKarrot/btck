// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <cstddef>
#include <cstdint>

#include <btck/btck.h>
#include <script/interpreter.h>

#include "script_pubkey.hpp"
#include "transaction.hpp"
#include "transaction_output.hpp"

namespace {

enum kernel_ScriptFlags
{
  kernel_SCRIPT_FLAGS_VERIFY_NONE = 0,
  kernel_SCRIPT_FLAGS_VERIFY_P2SH = (1U << 0),                  //!< evaluate P2SH (BIP16) subscripts
  kernel_SCRIPT_FLAGS_VERIFY_DERSIG = (1U << 2),                //!< enforce strict DER (BIP66) compliance
  kernel_SCRIPT_FLAGS_VERIFY_NULLDUMMY = (1U << 4),             //!< enforce NULLDUMMY (BIP147)
  kernel_SCRIPT_FLAGS_VERIFY_CHECKLOCKTIMEVERIFY = (1U << 9),   //!< enable CHECKLOCKTIMEVERIFY (BIP65)
  kernel_SCRIPT_FLAGS_VERIFY_CHECKSEQUENCEVERIFY = (1U << 10),  //!< enable CHECKSEQUENCEVERIFY (BIP112)
  kernel_SCRIPT_FLAGS_VERIFY_WITNESS = (1U << 11),              //!< enable WITNESS (BIP141)

  kernel_SCRIPT_FLAGS_VERIFY_TAPROOT = (1U << 17),  //!< enable TAPROOT (BIPs 341 & 342)
  kernel_SCRIPT_FLAGS_VERIFY_ALL = kernel_SCRIPT_FLAGS_VERIFY_P2SH | kernel_SCRIPT_FLAGS_VERIFY_DERSIG |
    kernel_SCRIPT_FLAGS_VERIFY_NULLDUMMY | kernel_SCRIPT_FLAGS_VERIFY_CHECKLOCKTIMEVERIFY |
    kernel_SCRIPT_FLAGS_VERIFY_CHECKSEQUENCEVERIFY | kernel_SCRIPT_FLAGS_VERIFY_WITNESS |
    kernel_SCRIPT_FLAGS_VERIFY_TAPROOT
};
enum kernel_ScriptVerifyStatus
{
  kernel_SCRIPT_VERIFY_OK = 0,
  kernel_SCRIPT_VERIFY_ERROR_TX_INPUT_INDEX,  //!< The provided input index is out of range of the actual number of inputs of the transaction.
  kernel_SCRIPT_VERIFY_ERROR_INVALID_FLAGS,              //!< The provided bitfield for the flags was invalid.
  kernel_SCRIPT_VERIFY_ERROR_INVALID_FLAGS_COMBINATION,  //!< The flags very combined in an invalid way.
  kernel_SCRIPT_VERIFY_ERROR_SPENT_OUTPUTS_REQUIRED,  //!< The taproot flag was set, so valid spent_outputs have to be provided.
  kernel_SCRIPT_VERIFY_ERROR_SPENT_OUTPUTS_MISMATCH,  //!< The number of spent outputs does not match the number of inputs of the tx.
};

auto verify_flags(unsigned int flags) -> bool
{
  return (flags & ~(kernel_SCRIPT_FLAGS_VERIFY_ALL)) == 0;
}

auto is_valid_flag_combination(unsigned int flags) -> bool
{
  if (flags & SCRIPT_VERIFY_CLEANSTACK && ~flags & (SCRIPT_VERIFY_P2SH | SCRIPT_VERIFY_WITNESS)) {
    return false;
  }
  if (flags & SCRIPT_VERIFY_WITNESS && ~flags & SCRIPT_VERIFY_P2SH) {
    return false;
  }
  return true;
}

}  // namespace

auto btck_Verify(
  btck_ScriptPubkey const* const script_pubkey,
  std::int64_t const amount,
  btck_Transaction const* const tx_to,
  btck_TransactionOutput const* const* const spent_outputs_,
  std::size_t const spent_outputs_len,
  unsigned int const input_index,
  unsigned int const flags,
  int* status
) -> bool
{
  if (!verify_flags(flags)) {
    if (status != nullptr) {
      *status = kernel_SCRIPT_VERIFY_ERROR_INVALID_FLAGS;
    }
    return false;
  }

  if (!is_valid_flag_combination(flags)) {
    if (status != nullptr) {
      *status = kernel_SCRIPT_VERIFY_ERROR_INVALID_FLAGS_COMBINATION;
    }
    return false;
  }

  if (flags & kernel_SCRIPT_FLAGS_VERIFY_TAPROOT && spent_outputs_ == nullptr) {
    if (status != nullptr) {
      *status = kernel_SCRIPT_VERIFY_ERROR_SPENT_OUTPUTS_REQUIRED;
    }
    return false;
  }

  CTransaction const& tx = *tx_to->transaction;

  std::vector<CTxOut> spent_outputs;
  if (spent_outputs_ != nullptr) {
    if (spent_outputs_len != tx.vin.size()) {
      if (status != nullptr) {
        *status = kernel_SCRIPT_VERIFY_ERROR_SPENT_OUTPUTS_MISMATCH;
      }
      return false;
    }

    spent_outputs.reserve(spent_outputs_len);
    for (auto const& out : std::span{spent_outputs_, spent_outputs_len}) {
      spent_outputs.push_back(out->tx_out);
    }
  }

  if (input_index >= tx.vin.size()) {
    if (status != nullptr) {
      *status = kernel_SCRIPT_VERIFY_ERROR_TX_INPUT_INDEX;
    }
    return false;
  }

  PrecomputedTransactionData txdata{tx};

  if (spent_outputs_ != nullptr && flags & kernel_SCRIPT_FLAGS_VERIFY_TAPROOT) {
    txdata.Init(tx, std::move(spent_outputs));
  }

  return VerifyScript(
    tx.vin[input_index].scriptSig, script_pubkey->script, &tx.vin[input_index].scriptWitness, flags,
    TransactionSignatureChecker(&tx, input_index, amount, txdata, MissingDataBehavior::FAIL), nullptr
  );
}
