// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <string>
#include <system_error>

#include <btck/btck.hpp>

namespace {

struct verify_error_category : std::error_category
{
  [[nodiscard]] auto name() const noexcept -> char const* override;
  [[nodiscard]] auto message(int ev) const -> std::string override;
};

auto verify_error_category::name() const noexcept -> char const*
{
  return "VerificationError";
}

auto verify_error_category::message(int ev) const -> std::string
{
  switch (static_cast<btck::verification_error>(ev)) {
    case btck::verification_error::tx_input_index:
      return "The provided input index is out of range of the actual number of "
             "inputs of the transaction.";

    case btck::verification_error::invalid_flags:
      return "The provided bitfield for the flags was invalid.";

    case btck::verification_error::invalid_flags_combination:
      return "The flags very combined in an invalid way.";

    case btck::verification_error::spent_outputs_required:
      return "The taproot flag was set, so valid spent_outputs have to be "
             "provided.";

    case btck::verification_error::spent_outputs_mismatch:
      return "The number of spent outputs does not match the number of inputs "
             "of the tx.";

    default:
      return "(unrecognized error)";
  }
}

auto const category = verify_error_category{};

}  // namespace

auto btck::make_error_code(btck::verification_error err) -> std::error_code
{
  return {static_cast<int>(err), category};
}
