
function(add_test name)
  add_library(${name} ${name}.cc)
  add_kernel(
    NAME ${name}
    LIBS ${name} io
    )
endfunction()

set(tests global-ctors-dtors)
list(APPEND tests initializers-operators)
list(APPEND tests virtuals)
list(APPEND tests heap)
list(APPEND tests rtti)
list(APPEND tests local-statics)
list(APPEND tests trivial-exception)
list(APPEND tests throw-clean-up-rethrow)
list(APPEND tests ccompat)
list(APPEND tests numeric-limits)
list(APPEND tests type-traits)

list(LENGTH tests test_length)
math(EXPR test_bound ${test_length}-1)

foreach(i RANGE 0 ${test_bound})
  list(GET tests ${i} name)
  if(i LESS 10)
    add_test(test-0${i}-${name})
  else()
    add_test(test-${i}-${name})
  endif()
endforeach()
