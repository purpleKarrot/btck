# Copyright (c) 2025-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

find_package(Lua REQUIRED)

add_library(btck-lua MODULE
  btck.c
  #src/block.c
  #src/chain.c
  #src/script_pubkey.c
  #src/transaction.c
  #src/transaction_output.c
  )

target_link_libraries(btck-lua PRIVATE btck::btck)

set_target_properties(btck-lua PROPERTIES
  C_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN ON
  OUTPUT_NAME btck
  PREFIX ""
  )

install(TARGETS btck-lua
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/lua/${LUA_VERSION_MAJOR}.${LUA_VERSION_MINOR}
  )
