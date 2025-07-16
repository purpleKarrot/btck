# Copyright (c) 2025-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

enable_language(Swift)

add_library(btck-swift SHARED
  btck.swift
  btck+.swift
  )

target_compile_options(btck-swift PRIVATE
  -enable-library-evolution
  -enable-testing # TODO: add generator expression?
  )

target_include_directories(btck-swift PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
  )

target_link_libraries(btck-swift PRIVATE
  btck::btck
  )

set_target_properties(btck-swift PROPERTIES
  Swift_MODULE_NAME btck
  )
