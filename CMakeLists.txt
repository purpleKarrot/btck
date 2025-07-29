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

list(APPEND CMAKE_MODULE_PATH "${BtcK_SOURCE_DIR}/cmake/modules")

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

add_subdirectory(test/c)
add_subdirectory(test/cpp)

option(BTCK_ENABLE_GO "Build Go extension" ON)
if(BTCK_ENABLE_GO)
  add_subdirectory(test/go)
endif()

option(BTCK_ENABLE_JAVA "Build Java extension" OFF)
if(BTCK_ENABLE_JAVA)
  add_subdirectory(bindings/java)
  # add_subdirectory(test/java)
endif()

option(BTCK_ENABLE_LUA "Build Lua extension" OFF)
if(BTCK_ENABLE_LUA)
  add_subdirectory(bindings/lua)
  # add_subdirectory(test/lua)
endif()

option(BTCK_ENABLE_PYTHON "Build python extension" ON)
if(BTCK_ENABLE_PYTHON)
  add_subdirectory(bindings/python)
  add_subdirectory(test/python)
endif()

option(BTCK_ENABLE_VALA "Enable Vala/Genie" OFF)
if(BTCK_ENABLE_PYTHON)
  add_subdirectory(bindings/vala)
  add_subdirectory(test/genie)
  add_subdirectory(test/vala)
endif()

add_library(btck SHARED)
add_library(BtcK::btck ALIAS btck)

target_sources(btck
  PUBLIC FILE_SET HEADERS BASE_DIRS include FILES
    include/btck/btck.h
  PRIVATE
    src/util/error.c
    src/util/error.cpp
    src/block.cpp
    src/chain.cpp
    src/script_pubkey.cpp
    src/transaction.cpp
    src/transaction_output.cpp
    src/verify.cpp
  )

target_compile_features(btck PRIVATE cxx_std_20)

set_target_properties(btck PROPERTIES
  CXX_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN ON
  VERSION ${BtcK_VERSION}
  SOVERSION ${BtcK_VERSION_MAJOR}
  DLL_NAME_WITH_SOVERSION ON
  GNUtoMS ON
  )

# bitcoin does not properly set the interface include directories.
target_include_directories(btck PRIVATE
  ${bitcoin_SOURCE_DIR}/src
  )

target_link_libraries(btck PRIVATE
  bitcoinkernel
  )

add_library(btck-cpp INTERFACE)
add_library(BtcK::btck-cpp ALIAS btck-cpp)

target_sources(btck-cpp
  PUBLIC FILE_SET HEADERS BASE_DIRS include FILES
    include/btck/btck.hpp
  )

target_compile_features(btck-cpp INTERFACE cxx_std_23)
target_link_libraries(btck-cpp INTERFACE BtcK::btck)

install(TARGETS btck btck-cpp EXPORT btck-targets
  FILE_SET HEADERS
  )

install(EXPORT btck-targets DESTINATION share/cmake
  FILE btck-config.cmake
  NAMESPACE BtcK::
  )

include(CMakePackageConfigHelpers)
write_basic_package_version_file(btck-config-version.cmake
  COMPATIBILITY SameMajorVersion
  )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/btck-config-version.cmake
  DESTINATION share/cmake
  )
