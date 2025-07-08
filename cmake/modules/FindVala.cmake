# Copyright (c) 2025-present The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

find_program(VALA_EXECUTABLE NAMES valac)
mark_as_advanced(VALA_EXECUTABLE)

if(VALA_EXECUTABLE)
  execute_process(COMMAND ${VALA_EXECUTABLE} "--version"
    OUTPUT_VARIABLE VALA_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
  string(REPLACE "Vala " "" VALA_VERSION "${VALA_VERSION}")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Vala
  REQUIRED_VARS VALA_EXECUTABLE
  VERSION_VAR VALA_VERSION
  )

function(vala_sources target)
  cmake_parse_arguments(PARSE_ARGV 1 ARGS "" "" "SOURCES;PACKAGES;OPTIONS")

  # get_target_property(type "${target}" TYPE)

  set(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${target}_vala)
  file(MAKE_DIRECTORY "${DIRECTORY}/public")

  # set(dir_args
  #   "--basedir=${CMAKE_CURRENT_SOURCE_DIR}"
  #   "--directory=${DIRECTORY}"
  #   )

  # set(vala_pkg_opts "")
  # foreach(pkg IN LISTS ARGS_PACKAGES)
  #   list(APPEND vala_pkg_opts "--pkg=${pkg}")
  # endforeach()

  # set(vala_define_opts "")
  # foreach(def IN LISTS ARGS_DEFINITIONS)
  #   list(APPEND vala_define_opts "--define=${def}")
  # endforeach()

  # fast vapi
  foreach(src IN LISTS ARGS_SOURCES ARGS_UNPARSED_ARGUMENTS)
    string(REGEX REPLACE "\.(gs|vala)$" ".vapi" vapi "${src}")
    file(REAL_PATH "${src}" abs_src)
    add_custom_command(OUTPUT "${DIRECTORY}/${vapi}"
      COMMAND "${VALA_EXECUTABLE}" "--fast-vapi=${DIRECTORY}/${vapi}" "${abs_src}"
      MAIN_DEPENDENCY "${abs_src}"
      )
  endforeach()

  # set(interface_headers)
  # set(private_headers)
  set(private_sources)

  foreach(src IN LISTS ARGS_SOURCES ARGS_UNPARSED_ARGUMENTS)
    set(vapi_args)
    set(vapi_deps)
    foreach(other IN LISTS ARGS_SOURCES ARGS_UNPARSED_ARGUMENTS)
      if(NOT src STREQUAL other)
        string(REGEX REPLACE "\.(gs|vala)$" ".vapi" vapi "${other}")
        list(APPEND vapi_args "--use-fast-vapi=${DIRECTORY}/${vapi}")
        list(APPEND vapi_deps "${DIRECTORY}/${vapi}")
      endif()
    endforeach()

    string(REGEX REPLACE "\.(gs|vala)$" ".c" c_file "${src}")
    string(REGEX REPLACE "\.(gs|vala)$" ".h" h_file "${src}")

    file(REAL_PATH "${src}" abs_src)
    add_custom_command(
      OUTPUT
        "${DIRECTORY}/${c_file}"
        # "${DIRECTORY}/${h_file}"
        # "${DIRECTORY}/public/${h_file}"
      COMMAND "${VALA_EXECUTABLE}"
      ARGS
        --ccode
        # "--header=public/${h_file}"
        # "--internal-header=${h_file}"
        # ${dir_args}
        ${vapi_args}
        # ${vala_pkg_opts}
        # ${vala_define_opts}
        ${abs_src}
        ${ARGS_OPTIONS}
      WORKING_DIRECTORY "${DIRECTORY}"
      DEPENDS ${abs_src} ${vapi_deps}
      )

    # list(APPEND interface_headers "${DIRECTORY}/public/${h_file}")
    # list(APPEND private_headers   "${DIRECTORY}/${h_file}")
    list(APPEND private_sources   "${DIRECTORY}/${c_file}")
  endforeach()

  target_sources(${target}
    # INTERFACE FILE_SET HEADERS
    #   BASE_DIRS "${DIRECTORY}/public"
    #   FILES ${interface_headers}
    # PRIVATE FILE_SET private_headers TYPE HEADERS
    #   BASE_DIRS "${DIRECTORY}"
    #   FILES ${private_headers}
    PRIVATE ${private_sources}
    )

  set_source_files_properties(${private_sources} PROPERTIES
    SKIP_LINTING ON
    )
endfunction()
