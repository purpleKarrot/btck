# Copyright (c) 2025-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

add_test(NAME btck.rust
  COMMAND cargo test
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  )

set_property(TEST btck.rust APPEND PROPERTY ENVIRONMENT
  "BTCK_LIBDIR=${BtcK_BINARY_DIR}"
  "CARGO_TARGET_DIR=${CMAKE_CURRENT_BINARY_DIR}/btck.rust.target"
  )

set_property(TEST btck.rust APPEND PROPERTY ENVIRONMENT_MODIFICATION
  "LD_LIBRARY_PATH=path_list_prepend:${BtcK_BINARY_DIR}"
  "PATH=path_list_prepend:${BtcK_BINARY_DIR}"
  )
