# Copyright (c) 2025-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

find_program(SPHINX_EXECUTABLE
  NAMES sphinx-build
  DOC "Sphinx Documentation Builder (sphinx-doc.org)"
  )

set(SPHINX_FLAGS "" CACHE STRING "Flags to pass to sphinx-build")
separate_arguments(sphinx_flags UNIX_COMMAND "${SPHINX_FLAGS}")

mark_as_advanced(SPHINX_TEXT)
mark_as_advanced(SPHINX_FLAGS)

if(NOT SPHINX_EXECUTABLE)
  message(FATAL_ERROR "SPHINX_EXECUTABLE (sphinx-build) is not found!")
endif()

configure_file(conf.py.in conf.py @ONLY)

add_custom_command(OUTPUT btck_doc_html
  COMMAND ${SPHINX_EXECUTABLE}
    -b html
    -c ${CMAKE_CURRENT_BINARY_DIR}
    -d ${CMAKE_CURRENT_BINARY_DIR}/doctrees
    ${sphinx_flags}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}/html
  COMMENT "sphinx-build html ..."
  VERBATIM
  )

set_property(SOURCE btck_doc_html PROPERTY SYMBOLIC 1)
add_custom_target(documentation ALL DEPENDS btck_doc_html)
