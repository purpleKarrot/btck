// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "error.hpp"

#include <btck/btck.h>

#include <exception>
#include <new>
#include <system_error>

namespace util {

auto TranslateException() -> BtcK_Error*
{
  try {
    throw;
  }
  catch (std::bad_alloc const& e) {
    return BtcK_Error_New(-1, "Memory", e.what());
  }
  catch (std::system_error const& e) {
    auto const& code = e.code();
    return BtcK_Error_New(code.value(), code.category().name(), e.what());
  }
  catch (std::exception const& e) {
    return BtcK_Error_New(-1, "Unknown", e.what());
  }
  catch (...) {
    return BtcK_Error_New(-1, "Unknown", "Unknown exception");
  }
}

}  // namespace util
