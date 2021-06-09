
#include "libsupcxx/io/printf.hh"

#include <cstddef>
#include <cfloat>
#include <ciso646>
#include <climits>
#include <cstdalign>
#include <cstdarg>
#include <cstdbool>
#include <cstdint>
#include <cstdlib>

void printArgs(int num, ...)
{
  va_list ap;
  int i;

  io::printf("printArgs(%d", num);
  va_start(ap, num); 
  for (i = 0; i < num; ++i) {
    io::printf(", %d", va_arg(ap, int));
  }
  va_end(ap);
  io::printf(")\n");
}

class CtorDtorTest {
public:
  CtorDtorTest() {
    io::printf("Constructing global object\n");
  }

  ~CtorDtorTest() {
    io::printf("Destructing global object\n");
  }
};

CtorDtorTest tmp;

void atExitFunc(void) {
  io::printf("At exit function called\n");
}

void main(const char *cmdline) {
  (void) tmp;
  io::printf("glibcxx datestamp: %u\n", __GLIBCXX__);
  io::printf("glibcxx release: %u\n", _GLIBCXX_RELEASE);
  io::printf("sizeof(std::size_t): %u\n", sizeof(std::size_t));
  io::printf("float eval method: %d\n", FLT_EVAL_METHOD);
  io::printf("ULLONG_MAX: %llu\n", ULLONG_MAX);
  io::printf("__alignas_is_defined: %d\n", __alignas_is_defined);
  printArgs(5, 1, 2, 3, 4, 5);
  io::printf("__bool_true_false_are_defined: %d\n", __bool_true_false_are_defined);
  io::printf("sizeof(std::uint8_t): %u\n", sizeof(std::uint8_t));
  std::atexit(atExitFunc);
  std::exit(0);
}
