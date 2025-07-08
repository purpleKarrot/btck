# Copyright (c) 2025-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

find_package(PkgConfig REQUIRED)
find_package(Vala REQUIRED)

pkg_check_modules(GLib REQUIRED IMPORTED_TARGET glib-2.0)

add_executable(btck.test.vala)
vala_sources(btck.test.vala
  SOURCES
    main.vala
    transaction.vala
  OPTIONS
    "${BtcK_SOURCE_DIR}/bindings/vala/btck.vapi"
  )

target_link_libraries(btck.test.vala PRIVATE
  BtcK::btck
  PkgConfig::GLib
  )

# TODO: https://gitlab.kitware.com/cmake/cmake/-/issues/26920
add_test(NAME btck.vala COMMAND btck.test.vala)
