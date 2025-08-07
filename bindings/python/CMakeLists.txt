# Copyright (c) 2025-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

find_package(Python3 3.12 REQUIRED COMPONENTS Development.Module)

python3_add_library(btck-python WITH_SOABI
  btck.c
  src/_bytes_writer.c
  src/_error.c
  src/_slice.c
  src/block.c
  src/block_hash.c
  src/chain.c
  src/script_pubkey.c
  src/transaction.c
  src/transaction_output.c
  src/verification_flags.c
  )

target_link_libraries(btck-python PRIVATE BtcK::btck)

set_target_properties(btck-python PROPERTIES
  C_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN ON
  OUTPUT_NAME btck
  )

install(TARGETS btck-python
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/python${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}/site-packages
  )

install(FILES btck.pyi
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/python${Python3_VERSION_MAJOR}.${Python3_VERSION_MINOR}/site-packages
  )
