# Copyright (c) 2025-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(btck.test.cpp
  block.cpp
  transaction.cpp
  )

target_link_libraries(btck.test.cpp PRIVATE
  btck::btck-cpp
  GTest::gtest_main
  )

gtest_discover_tests(btck.test.cpp
  DISCOVERY_MODE PRE_TEST
  TEST_PREFIX btck.cpp.
  )
