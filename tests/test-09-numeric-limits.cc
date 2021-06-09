
#include "libsupcxx/io/printf.hh"

#include <limits>
#include <cstdint>

void main(const char *cmdline) {
  io::printf("Hello numeric_limits\n");
  io::printf("std::numeric_limits<std::uint64_t>::max(): %llu\n",
             std::numeric_limits<std::uint64_t>::max());
  io::printf("std::numeric_limits<std::uint64_t>::digits: %d\n",
             std::numeric_limits<std::uint64_t>::digits);

}
