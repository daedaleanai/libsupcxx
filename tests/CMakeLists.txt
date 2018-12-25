
function(add_test name)
  add_library(${name} ${name}.cc)
  add_kernel(
    NAME ${name}.elf
    LIBS ${name} io
    )
endfunction()

set(tests global-ctors-dtors;initializers-operators)

foreach(i RANGE 0 1)
  list(GET tests ${i} name)
  if(i LESS 10)
    add_test(test-0${i}-${name})
  else()
    add_test(test-${i}-${name})
  endif()
endforeach()
