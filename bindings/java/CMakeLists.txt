# Copyright (c) 2025-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

find_package(Java REQUIRED)
find_package(JNI REQUIRED)
include(UseJava)

add_jar(btck-java
    src/Block.java
    src/Chain.java
    src/ScriptPubkey.java
    src/Transaction.java
    src/TransactionOutput.java
  GENERATE_NATIVE_HEADERS btck-java-headers
  )

add_library(btck-java-native MODULE
  src/Block.c
  src/Chain.c
  src/ScriptPubkey.c
  src/Transaction.c
  src/TransactionOutput.c
  )

target_link_libraries(btck-java-native PRIVATE
  btck-java-headers
  btck::btck
  )
