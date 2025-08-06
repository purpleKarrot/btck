# Copyright (c) 2025-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

find_program(NODE_EXECUTABLE
  NAMES node
  DOC "NodeJS executable"
  )

if(NOT NODE_EXECUTABLE)
  message(NODE_EXECUTABLE "NODE_EXECUTABLE (node) is not found!")
endif()

add_test(NAME btck.nodejs COMMAND ${NODE_EXECUTABLE} --test)
set_tests_properties(btck.nodejs PROPERTIES
  ENVIRONMENT "BTCK_NODE_DIR=$<TARGET_FILE_DIR:btck-node>"
  WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
  )
