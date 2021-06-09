
#include "libsupcxx/io/printf.hh"

#include <type_traits>
#include <cstdint>

struct Test {
  std::uint16_t test = 42;
};

void main(const char *cmdline) {
  io::printf("Is int tivially assignable to const Test: %d!\n",
             std::is_trivially_assignable<const Test, int>::value);
  io::printf("Is const Test& trivially assignable to Test&: %d!\n",
             std::is_trivially_assignable<Test&, const Test&>::value);
}
