# Copyright (c) 2025-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

# https://gitlab.kitware.com/cmake/cmake/-/issues/27057
set(CMAKE_GNUtoMS ON)

cmake_minimum_required(VERSION 3.31...4.0)
project(BtcK
  VERSION 0.21.0
  DESCRIPTION "BTC Kernel API for various languages"
  LANGUAGES C CXX
  )

# https://gitlab.kitware.com/cmake/cmake/-/issues/27056
if(WIN32)
  set(CMAKE_IMPORT_LIBRARY_PREFIX "")
  set(CMAKE_SHARED_LIBRARY_PREFIX "")
  set(CMAKE_SHARED_MODULE_PREFIX  "")
  set(CMAKE_STATIC_LIBRARY_PREFIX "")
endif()

enable_testing()
include(CPack)
include(CTestUseLaunchers)
include(GNUInstallDirs)

block(PROPAGATE bitcoin_SOURCE_DIR)
  set(CMAKE_C_CLANG_TIDY "")
  set(CMAKE_C_INCLUDE_WHAT_YOU_USE "")
  set(CMAKE_CXX_CLANG_TIDY "")
  set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE "")
  set(CMAKE_LINK_WHAT_YOU_USE "")

  set(ENABLE_WALLET OFF)
  set(BUILD_WALLET_TOOL OFF)
  set(BUILD_BENCH OFF)
  set(BUILD_CLI OFF)
  set(BUILD_DAEMON OFF)
  set(BUILD_BITCOIN_BIN OFF)
  set(BUILD_SHARED_LIBS OFF)
  set(BUILD_TESTS OFF)
  set(BUILD_TX OFF)
  set(BUILD_UTIL OFF)
  set(ENABLE_EXTERNAL_SIGNER OFF)
  set(BUILD_KERNEL_LIB ON)

  include(FetchContent)
  FetchContent_Declare(bitcoin EXCLUDE_FROM_ALL
    GIT_REPOSITORY https://github.com/bitcoin/bitcoin.git
    GIT_TAG        v29.0
    GIT_SHALLOW    ON
    )
  FetchContent_MakeAvailable(bitcoin)
  FetchContent_GetProperties(bitcoin SOURCE_DIR bitcoin_SOURCE_DIR)
endblock()

option(BTCK_RUN_IWYU "Run include-what-you-use with the compiler." OFF)
if(BTCK_RUN_IWYU)
  find_program(IWYU_COMMAND NAMES include-what-you-use iwyu)
  if(NOT IWYU_COMMAND)
    message(FATAL_ERROR "BTCK_RUN_IWYU is ON but include-what-you-use is not found!")
  endif()
  foreach(lang C CXX)
    set(CMAKE_${lang}_INCLUDE_WHAT_YOU_USE ${IWYU_COMMAND}
      -Xiwyu --mapping_file=${BtcK_SOURCE_DIR}/.iwyu
      -w
      )
  endforeach()
endif()

include(CMakePackageConfigHelpers)
write_basic_package_version_file(btck-config-version.cmake
  COMPATIBILITY SameMajorVersion
  )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/btck-config-version.cmake
  DESTINATION share/cmake
  )
