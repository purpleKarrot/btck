// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include <type_traits>

struct BtcK_Error;

namespace util {

auto TranslateException() -> BtcK_Error*;

template <typename Function> auto WrapFn(BtcK_Error** err, Function&& function)
{
  try {
    return function();
  }
  catch (...) {
    if (err != nullptr) {
      *err = TranslateException();
    }

    using Ret = decltype(function());
    if constexpr (!std::is_void_v<Ret>) {
      return Ret{};
    }
  }
}

}  // namespace util
