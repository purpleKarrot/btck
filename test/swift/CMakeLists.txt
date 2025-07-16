# Copyright (c) 2025-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

enable_language(Swift)

include(FetchContent)
FetchContent_Declare(SwiftTesting
  GIT_REPOSITORY https://github.com/swiftlang/swift-testing.git
  GIT_TAG main
  )
FetchContent_MakeAvailable(SwiftTesting)

add_executable(btck.test.swift
  test_main.swift
  transaction.swift
  )

set_target_properties(btck.test.swift PROPERTIES
  SUFFIX .swift-testing
  )

target_link_libraries(btck.test.swift PRIVATE
  btck-swift
  Testing
  )

add_test(NAME btck.swift COMMAND btck.test.swift)
