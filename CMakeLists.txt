# Copyright (c) 2025-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

cmake_minimum_required(VERSION 3.31...4.0)
project(btck
  VERSION 0.21.0
  DESCRIPTION "BTC Kernel API for various languages"
  LANGUAGES C CXX
  )

enable_testing()
include(CPack)
include(CTestUseLaunchers)
include(GNUInstallDirs)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(btck-config-version.cmake
  COMPATIBILITY SameMajorVersion
  )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/btck-config-version.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/btck
  )
