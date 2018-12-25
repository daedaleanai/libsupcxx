
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
    boot_last
    -Wl,--no-whole-archive
    ${add_kernel_LIBS}
    runcxx
    gcc
    )
endfunction(add_kernel)
