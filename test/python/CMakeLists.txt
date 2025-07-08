# Copyright (c) 2025-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

find_package(Python3 3.12 REQUIRED COMPONENTS Interpreter)
find_package(Pytest 8.4 REQUIRED)

# WARNING: pytest_discover_tests discovers tests at build time!
pytest_discover_tests(btck.python
  LIBRARY_PATH_PREPEND $<TARGET_FILE_DIR:btck::btck>
  PYTHON_PATH_PREPEND $<TARGET_FILE_DIR:btck-python>
  TRIM_FROM_NAME "^(Test|test_)"
  DEPENDS btck-python
  )

find_program(stubtest_COMMAND NAMES stubtest)
if(stubtest_COMMAND)
  add_test(NAME btck.python.mypy COMMAND ${stubtest_COMMAND} --concise btck)
  set_property(TEST btck.python.mypy PROPERTY ENVIRONMENT
    "PYTHONPATH=$<TARGET_FILE_DIR:btck-python>"
    "MYPYPATH=$<TARGET_PROPERTY:btck-python,SOURCE_DIR>"
    )
endif()
