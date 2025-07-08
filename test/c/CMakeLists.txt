# Copyright (c) 2025-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

find_package(cmocka REQUIRED)

add_executable(btck.test.c)
target_sources(btck.test.c PRIVATE
  main.c
  )
target_link_libraries(btck.test.c PRIVATE btck::btck cmocka::cmocka)
add_test(NAME btck.c COMMAND btck.test.c)
