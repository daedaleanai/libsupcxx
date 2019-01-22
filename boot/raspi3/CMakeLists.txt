
add_gcc_object(CRT2 crtbegin.o crt2.o)
add_gcc_object(CRT3 crtend.o crt3.o)

add_library(
  boot_first STATIC
  crt0.S
  ${CRT2}
  setup.cc
  )

add_library(
  boot_last STATIC
  ${CRT3}
  io.cc
  )
