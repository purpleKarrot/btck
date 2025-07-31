// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <btck/btck.h>
#include <script/interpreter.h>

#include <btck/btck.hpp>
#include <cstddef>
#include <cstdint>
#include <ranges>
#include <span>
#include <system_error>
#include <utility>
#include <vector>

#include "primitives/transaction.h"
#include "script_pubkey.hpp"
#include "transaction.hpp"
#include "transaction_output.hpp"  // IWYU pragma: keep
#include "util/error.hpp"

class CScript;

namespace {

auto verify(
  CScript const& script_pubkey, std::int64_t const amount,
  CTransaction const& tx, std::vector<CTxOut> spent_outputs,
  unsigned int const input_index, BtcK_VerificationFlags flags)
{
  if ((flags & ~BtcK_VerificationFlags_ALL) != 0) {
    throw std::system_error(btck::verification_error::invalid_flags);
  }

  bool const cleanstack = (flags & SCRIPT_VERIFY_CLEANSTACK) != 0;
  bool const p2sh = (flags & SCRIPT_VERIFY_P2SH) != 0;
  bool const witness = (flags & SCRIPT_VERIFY_WITNESS) != 0;
  bool const taproot = (flags & SCRIPT_VERIFY_TAPROOT) != 0;

  if ((cleanstack && !p2sh && !witness) || (witness && !p2sh)) {
    throw std::system_error(
      btck::verification_error::invalid_flags_combination);
  }

  if (taproot && spent_outputs.empty()) {
    throw std::system_error(btck::verification_error::spent_outputs_required);
  }

  if (!spent_outputs.empty() && spent_outputs.size() != tx.vin.size()) {
    throw std::system_error(btck::verification_error::spent_outputs_mismatch);
  }

  if (input_index >= tx.vin.size()) {
    throw std::system_error(btck::verification_error::tx_input_index);
  }

  PrecomputedTransactionData txdata{tx};

  if (taproot) {
    txdata.Init(tx, std::move(spent_outputs));
  }

  return VerifyScript(
    tx.vin[input_index].scriptSig, script_pubkey,
    &tx.vin[input_index].scriptWitness, flags,
    TransactionSignatureChecker(
      &tx, input_index, amount, txdata, MissingDataBehavior::FAIL),
    nullptr);
}

}  // namespace

auto BtcK_Verify(
  struct BtcK_ScriptPubkey const* script_pubkey, int64_t amount,
  struct BtcK_Transaction const* tx,
  struct BtcK_TransactionOutput const* const* spent_outputs,
  std::size_t spent_outputs_len, unsigned int input_index,
  BtcK_VerificationFlags flags, struct BtcK_Error** err) -> bool
{
  return util::WrapFn(err, [=] {
    auto const spent_outputs_view =
      std::span{spent_outputs, spent_outputs_len} |
      std::views::transform([](auto const* out) { return out->tx_out; });
    return verify(
      script_pubkey->script, amount, *tx->transaction,
      std::vector(spent_outputs_view.begin(), spent_outputs_view.end()),
      input_index, flags);
  });
}
