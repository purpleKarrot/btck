# Copyright (c) 2025-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

add_test(NAME btck.go
  COMMAND go test
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  )

set_property(TEST btck.go APPEND PROPERTY ENVIRONMENT
  "CGO_CFLAGS=-I$<TARGET_PROPERTY:BtcK::btck,INTERFACE_INCLUDE_DIRECTORIES>"
  "CGO_LDFLAGS=-L${BtcK_BINARY_DIR}"
  )

set_property(TEST btck.go APPEND PROPERTY ENVIRONMENT_MODIFICATION
  "LD_LIBRARY_PATH=path_list_prepend:${BtcK_BINARY_DIR}"
  "PATH=path_list_prepend:${BtcK_BINARY_DIR}"
  )
