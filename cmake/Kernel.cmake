#-------------------------------------------------------------------------------
# Copyright (C) 2018-2019 Daedalean AG
#
# This library is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# Under Section 7 of GPL version 3, you are granted additional
# permissions described in the GCC Runtime Library Exception, version
# 3.1, as published by the Free Software Foundation.
#
# You should have received a copy of the GNU General Public License and
# a copy of the GCC Runtime Library Exception along with this program;
# see the files LICENSE and LICENSE.RUNTIME respectively.  If not, see
# <http://www.gnu.org/licenses/>.
#-------------------------------------------------------------------------------

macro(add_gcc_object variable obj name)
  execute_process(
    COMMAND ${CMAKE_C_COMPILER} ${CMAKE_C_FLAGS} -print-file-name=${obj}
    OUTPUT_VARIABLE OBJ_COMP
    )

  string(STRIP ${OBJ_COMP} OBJ_COMP)

  set(${variable} ${CMAKE_CURRENT_BINARY_DIR}/${name}${CMAKE_C_OUTPUT_EXTENSION})

  add_custom_command(
    OUTPUT ${${variable}}
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OBJ_COMP} ${${variable}}
    DEPENDS ${OBJ_COMP}
    COMMENT "Importing ${OBJ_COMP} as ${name}"
    )
endmacro(add_gcc_object)

function(add_kernel)
  set(oneValueArgs NAME)
  set(multiValueArgs LIBS)
  cmake_parse_arguments(add_kernel "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

  add_custom_command(
    OUTPUT ${add_kernel_NAME}-fake.c
    COMMAND ${CMAKE_COMMAND} -E touch ${add_kernel_NAME}-fake.c
    )

  add_executable(
    ${add_kernel_NAME}
    ${add_kernel_NAME}-fake.c
    )

  target_link_libraries(
    ${add_kernel_NAME}
    -Wl,--whole-archive
    boot_first
    -Wl,--no-whole-archive
    ${add_kernel_LIBS}
    supcxx
    gcc
    runcxx
    -Wl,--whole-archive
    boot_last
    -Wl,--no-whole-archive

    )
endfunction(add_kernel)
