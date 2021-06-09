
#include "libsupcxx/io/printf.hh"

class TestGlobalCtorDtor {
public:
  TestGlobalCtorDtor() {
    io::printf("Ctor called!\n");
  }
  ~TestGlobalCtorDtor() {
    io::printf("Dtor called!\n");
  }
};

TestGlobalCtorDtor tmp;

void main(const char *cmdline) {
  io::printf("Hello, Kernel World!\nAnd Hello Again!\n");
  io::printf("Kernel commandline: %s\n", cmdline);
}
