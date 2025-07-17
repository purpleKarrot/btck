# Copyright (c) 2025-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

find_program(GO_EXECUTABLE go DOC "Go executable")

if (GO_EXECUTABLE)
  execute_process(
    COMMAND ${GO_EXECUTABLE} version
    OUTPUT_VARIABLE GO_VERSION_STRING
    RESULT_VARIABLE RESULT
    )
  if (RESULT EQUAL 0)
    string(REGEX MATCH "([0-9]+\\.[0-9]+\(\\.[0-9]+\)?)"
      GO_VERSION_STRING "${GO_VERSION_STRING}"
      )
  endif()
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Go
  REQUIRED_VARS GO_EXECUTABLE
  VERSION_VAR GO_VERSION_STRING
  )
