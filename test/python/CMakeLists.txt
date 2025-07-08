# Copyright (c) 2025-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

find_package(Python3 3.12 REQUIRED COMPONENTS Interpreter)

add_test(NAME btck.python COMMAND pytest ${CMAKE_CURRENT_SOURCE_DIR})

# TODO: This should be generalized
set(dll_dirs "$<TARGET_RUNTIME_DLL_DIRS:btck-python>")
set(is_empty "$<STREQUAL:${dll_dirs},>")
set(env_op "PATH=path_list_append:")

set_property(TEST btck.python APPEND PROPERTY ENVIRONMENT_MODIFICATION
  "$<$<NOT:${is_empty}>:${env_op}$<JOIN:${dll_dirs},$<SEMICOLON>${env_op}>>"
  "PYTHONPATH=path_list_append:$<TARGET_FILE_DIR:btck-python>"
  )

#pytest_discover_tests(btck.python
#  LIBRARY_PATH_PREPEND $<TARGET_FILE_DIR:btck::btck>
#  PYTHON_PATH_PREPEND $<TARGET_FILE_DIR:btck-python>
#  TRIM_FROM_NAME "^(Test|test_)"
#  DEPENDS btck-python
#  )

find_program(stubtest_COMMAND NAMES stubtest)
if(stubtest_COMMAND)
  add_test(NAME btck.python.mypy COMMAND ${stubtest_COMMAND} --concise btck)
  set_property(TEST btck.python.mypy PROPERTY ENVIRONMENT
    "PYTHONPATH=$<TARGET_FILE_DIR:btck-python>"
    "MYPYPATH=$<TARGET_PROPERTY:btck-python,SOURCE_DIR>"
    )
endif()
