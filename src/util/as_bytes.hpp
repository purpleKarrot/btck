// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <span>

namespace util {

constexpr auto const AsBytes = [](auto const& ref, std::size_t* out_len) {
  auto const bytes = as_bytes(std::span{ref});
  if (out_len != nullptr) {
    *out_len = bytes.size();
  }
  return bytes.data();
};

}  // namespace util
