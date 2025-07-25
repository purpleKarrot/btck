// Copyright (c) 2025-present The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "error.hpp"

#include <exception>
#include <new>

#include <btck/btck.h>

namespace util {

auto TranslateException() -> BtcK_Error*
{
  try {
    throw;
  }
  catch (std::bad_alloc const& e) {
    return BtcK_Error_New("Memory", 0, e.what());
  }
  catch (std::exception const& e) {
    return BtcK_Error_New("Object", 2, e.what());
  }
  catch (...) {
    return BtcK_Error_New("Unknown", 0, "Unknown exception");
  }
}

}  // namespace util
