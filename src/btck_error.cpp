// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <btck/btck_error.hpp>
#include <string>

auto btck::verification_error_category() -> std::error_category const&
{
  static struct : std::error_category {
    [[nodiscard]] auto name() const noexcept -> char const* override
    {
      return "VerificationError";
    }

    [[nodiscard]] auto message(int ev) const -> std::string override
    {
      return BtcK_VerificationError_Message(ev);
    }
  } const category;

  return category;
}

auto btck::make_error_code(verification_error err) -> std::error_code
{
  return {static_cast<int>(err), verification_error_category()};
}
