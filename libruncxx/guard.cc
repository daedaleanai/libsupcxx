
#include <stdint.h>

// See: https://itanium-cxx-abi.github.io/cxx-abi/abi.html#once-ctor

namespace __cxxabiv1 {
extern "C" int __cxa_guard_acquire(uint64_t *guard) {
  if ((*guard) & 0xff) {
    return 0;
  }
  return 1;
}

extern "C" void __cxa_guard_release(uint64_t *guard) noexcept {
  *guard |= 1;
}

extern "C" void __cxa_guard_abort(uint64_t *guard) noexcept {
}
} // namespace __cxxabiv1
