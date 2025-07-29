// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <cstddef>
#include <cstdint>
#include <ranges>
#include <span>
#include <stdexcept>
#include <utility>
#include <vector>

#include <btck/btck.h>
#include <script/interpreter.h>

#include "primitives/transaction.h"
#include "script_pubkey.hpp"
#include "transaction.hpp"
#include "transaction_output.hpp"  // IWYU pragma: keep
#include "util/error.hpp"

class CScript;

namespace {

enum kernel_ScriptVerifyStatus
{
  kernel_SCRIPT_VERIFY_OK = 0,
  kernel_SCRIPT_VERIFY_ERROR_TX_INPUT_INDEX,  //!< The provided input index is out of range of the actual number of inputs of the transaction.
  kernel_SCRIPT_VERIFY_ERROR_INVALID_FLAGS,  //!< The provided bitfield for the flags was invalid.
  kernel_SCRIPT_VERIFY_ERROR_INVALID_FLAGS_COMBINATION,  //!< The flags very combined in an invalid way.
  kernel_SCRIPT_VERIFY_ERROR_SPENT_OUTPUTS_REQUIRED,  //!< The taproot flag was set, so valid spent_outputs have to be provided.
  kernel_SCRIPT_VERIFY_ERROR_SPENT_OUTPUTS_MISMATCH,  //!< The number of spent outputs does not match the number of inputs of the tx.
};

class VerifyError : public std::logic_error
{
public:
  VerifyError(int code)
    : std::logic_error{""}
  {}
};

inline auto verify(
  CScript const& script_pubkey,
  std::int64_t const amount,
  CTransaction const& tx,
  std::vector<CTxOut> spent_outputs,
  unsigned int const input_index,
  BtcK_ScriptVerify flags
)
{
  if ((flags & ~BtcK_ScriptVerify_ALL) != 0) {
    throw VerifyError(kernel_SCRIPT_VERIFY_ERROR_INVALID_FLAGS);
  }

  bool const cleanstack = (flags & SCRIPT_VERIFY_CLEANSTACK) != 0;
  bool const p2sh = (flags & SCRIPT_VERIFY_P2SH) != 0;
  bool const witness = (flags & SCRIPT_VERIFY_WITNESS) != 0;
  bool const taproot = (flags & SCRIPT_VERIFY_TAPROOT) != 0;

  if ((cleanstack && !p2sh && !witness) || (witness && !p2sh)) {
    throw VerifyError(kernel_SCRIPT_VERIFY_ERROR_INVALID_FLAGS_COMBINATION);
  }

  if (taproot && spent_outputs.empty()) {
    throw VerifyError(kernel_SCRIPT_VERIFY_ERROR_SPENT_OUTPUTS_REQUIRED);
  }

  if (!spent_outputs.empty() && spent_outputs.size() != tx.vin.size()) {
    throw VerifyError(kernel_SCRIPT_VERIFY_ERROR_SPENT_OUTPUTS_MISMATCH);
  }

  if (input_index >= tx.vin.size()) {
    throw VerifyError(kernel_SCRIPT_VERIFY_ERROR_TX_INPUT_INDEX);
  }

  PrecomputedTransactionData txdata{tx};

  if (taproot) {
    txdata.Init(tx, std::move(spent_outputs));
  }

  return VerifyScript(
    tx.vin[input_index].scriptSig, script_pubkey,
    &tx.vin[input_index].scriptWitness, flags,
    TransactionSignatureChecker(
      &tx, input_index, amount, txdata, MissingDataBehavior::FAIL
    ),
    nullptr
  );
}

}  // namespace

auto BtcK_Verify(
  struct BtcK_ScriptPubkey const* script_pubkey,
  int64_t amount,
  struct BtcK_Transaction const* tx,
  struct BtcK_TransactionOutput const* const* spent_outputs,
  std::size_t spent_outputs_len,
  unsigned int input_index,
  BtcK_ScriptVerify flags,
  struct BtcK_Error** err
) -> bool
{
  return util::WrapFn(err, [=] {
    auto const spent_outputs_view =
      std::span{spent_outputs, spent_outputs_len} |
      std::views::transform([](auto const* out) { return out->tx_out; });
    return verify(
      script_pubkey->script, amount, *tx->transaction,
      std::vector<CTxOut>(spent_outputs_view.begin(), spent_outputs_view.end()),
      input_index, flags
    );
  });
}
