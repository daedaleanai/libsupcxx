
#pragma once

#include <cstddef>

namespace __cxxabiv1 {
  // Maximum number of global destructors that the runtime can keep on the
  // record. There's no heap memory at this point, and we want to keep things
  // simple, so we're stuck with whatever we preconfigure here.
  const size_t GLIBCXX_NUM_GLOBAL_DTORS = 100;
} // namespace __cxxabiv1
