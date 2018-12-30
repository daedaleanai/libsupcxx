
#include <io/printf.hh>

void main(const char *cmdline) {
  io::printf("A trivial exception test\n");
  try {
    io::printf("Throwing 42\n");
    throw 42;
  } catch (int e) {
    io::printf("Cought: %d\n", e);
  }
}
