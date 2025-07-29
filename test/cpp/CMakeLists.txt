# Copyright (c) 2025-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

find_package(GTest REQUIRED)

add_executable(btck.test.cpp
  block.cpp
  transaction.cpp
  verify.cpp
  )

target_link_libraries(btck.test.cpp PRIVATE
  BtcK::btck-cpp
  GTest::gtest_main
  )

# TODO: https://gitlab.kitware.com/cmake/cmake/-/issues/26920
add_test(NAME btck.cpp COMMAND btck.test.cpp)
