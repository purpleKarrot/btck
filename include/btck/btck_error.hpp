// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <btck/btck.h>

#include <system_error>
#include <type_traits>

namespace btck {

enum class verification_error : BtcK_VerificationError {
  tx_input_index = BtcK_VerificationError_TX_INPUT_INDEX,
  invalid_flags = BtcK_VerificationError_INVALID_FLAGS,
  invalid_flags_combination = BtcK_VerificationError_INVALID_FLAGS_COMBINATION,
  spent_outputs_required = BtcK_VerificationError_SPENT_OUTPUTS_REQUIRED,
  spent_outputs_mismatch = BtcK_VerificationError_SPENT_OUTPUTS_MISMATCH,
};

auto verification_error_category() -> std::error_category const&;

auto make_error_code(verification_error err) -> std::error_code;

}  // namespace btck

template <>
struct std::is_error_code_enum<btck::verification_error> : std::true_type {};
