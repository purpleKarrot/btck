# Copyright (c) 2025-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

enable_language(Swift)

block()
  set(CMAKE_C_CLANG_TIDY "")
  set(CMAKE_C_INCLUDE_WHAT_YOU_USE "")
  set(CMAKE_CXX_CLANG_TIDY "")
  set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE "")
  set(CMAKE_LINK_WHAT_YOU_USE "")

  include(FetchContent)
  FetchContent_Declare(SwiftTesting
    GIT_REPOSITORY https://github.com/swiftlang/swift-testing.git
    GIT_TAG swift-6.1.1-RELEASE
    )
  FetchContent_MakeAvailable(SwiftTesting)
endblock()

add_executable(btck_test_swift
  test_main.swift
  transaction.swift
  )

target_compile_options(btck_test_swift PRIVATE
  -enable-experimental-feature Extern
  )

set_target_properties(btck_test_swift PROPERTIES
  SUFFIX .swift-testing
  )

target_link_libraries(btck_test_swift PRIVATE
  btck-swift
  Testing
  )

add_test(NAME btck.swift COMMAND btck_test_swift)
