# Copyright (c) 2025-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

# https://gitlab.kitware.com/cmake/cmake/-/issues/27057
set(CMAKE_GNUtoMS ON)

cmake_minimum_required(VERSION 3.31...4.0)
project(btck
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

# https://gitlab.kitware.com/cmake/cmake/-/issues/25686
function(add_test)
  _add_test(${ARGV})
  cmake_parse_arguments(PARSE_ARGV 0 arg "" NAME COMMAND)
  list(GET arg_COMMAND 0 command)
  if(TARGET "${command}" OR command MATCHES "::")
    set(dll_dirs "$<TARGET_RUNTIME_DLL_DIRS:${command}>")
    set(is_empty "$<STREQUAL:${dll_dirs},>")
    set(env_op "PATH=path_list_append:")
    set_property(TEST ${arg_NAME} APPEND PROPERTY ENVIRONMENT_MODIFICATION
      "$<$<NOT:${is_empty}>:${env_op}$<JOIN:${dll_dirs},$<SEMICOLON>${env_op}>>"
    )
  endif()
endfunction()

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


include(CMakePackageConfigHelpers)
write_basic_package_version_file(btck-config-version.cmake
  COMPATIBILITY SameMajorVersion
  )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/btck-config-version.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/btck
  )
