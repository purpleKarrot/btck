// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <btck/btck.h>  // IWYU pragma: associated
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
#include "script/script.h"
#include "util/api.hpp"
#include "util/error.hpp"

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

extern "C" {

auto BtcK_ScriptPubkey_New(
  void const* raw, std::size_t len, struct BtcK_Error** err)
  -> BtcK_ScriptPubkey*
{
  return util::WrapFn(err, [raw, len] {
    auto data = std::span{reinterpret_cast<std::uint8_t const*>(raw), len};
    return api::create<CScript>(data.begin(), data.end());
  });
}

auto BtcK_ScriptPubkey_Copy(
  BtcK_ScriptPubkey const* self, struct BtcK_Error** err) -> BtcK_ScriptPubkey*
{
  return api::copy(self, err);
}

void BtcK_ScriptPubkey_Free(BtcK_ScriptPubkey* self)
{
  api::free(self);
}

auto BtcK_ScriptPubkey_Equal(
  BtcK_ScriptPubkey const* left, BtcK_ScriptPubkey const* right) -> int
{
  return (api::get(left) == api::get(right)) ? 1 : 0;
}

auto BtcK_ScriptPubkey_ToBytes(
  const struct BtcK_ScriptPubkey* self, BtcK_WriteBytes write, void* userdata)
  -> int
{
  auto const bytes = as_bytes(std::span{api::get(self)});
  return write(bytes.data(), bytes.size(), userdata);
}

auto BtcK_ScriptPubkey_Verify(
  struct BtcK_ScriptPubkey const* script_pubkey, int64_t amount,
  struct BtcK_Transaction const* tx,
  struct BtcK_TransactionOutput const* const* spent_outputs,
  std::size_t spent_outputs_len, unsigned int input_index,
  BtcK_VerificationFlags flags, struct BtcK_Error** err) -> int
{
  return util::WrapFn(err, [=] {
    auto const spent_outputs_view =
      std::span{spent_outputs, spent_outputs_len} |
      std::views::transform([](auto const* out) { return api::get(out); });
    auto const result = verify(
      api::get(script_pubkey), amount, *api::get(tx),
      std::vector(spent_outputs_view.begin(), spent_outputs_view.end()),
      input_index, flags);
    return result ? 1 : 0;
  });
}

}  // extern "C"
