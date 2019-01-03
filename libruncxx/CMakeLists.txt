
add_library(
  runcxx STATIC
  dtors.cc
  memory.cc
  abort.cc
  guard.cc
  string.cc
  eh_alloc.cc
  eh_globals.cc
  exit.cc
)
