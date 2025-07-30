// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "error.hpp"

#include <exception>
#include <new>
#include <system_error>

#include <btck/btck.h>

namespace util {

auto TranslateException() -> BtcK_Error*
{
  try {
    throw;
  }
  catch (std::bad_alloc const& e) {
    return BtcK_Error_New("Memory", -1, e.what());
  }
  catch (std::system_error const& e) {
    auto const& code = e.code();
    return BtcK_Error_New(code.category().name(), code.value(), e.what());
  }
  catch (std::exception const& e) {
    return BtcK_Error_New("Unknown", -1, e.what());
  }
  catch (...) {
    return BtcK_Error_New("Unknown", -1, "Unknown exception");
  }
}

}  // namespace util
