
#include <stdint.h>
#include "../libsupcxx/src/exception_defines.h"
#include "../libsupcxx/src/cxxabi.h"

namespace __gnu_cxx {
  recursive_init_error::~recursive_init_error() noexcept { }
}

// See: https://itanium-cxx-abi.github.io/cxx-abi/abi.html#once-ctor
// The implementation below is not really compliant, because the GNU library
// uses the second byte of the guard to either throw an exception or abort if
// the initializer is called recursively.
namespace __cxxabiv1 {
extern "C" int __cxa_guard_acquire(uint64_t *guard) {
  if ((*guard) & 0x01) {
    return 0;
  }
  if ((*guard) & 0x0100) {
    _GLIBCXX_THROW_OR_ABORT(__gnu_cxx::recursive_init_error());
  }
  *guard |= 0x0100;
  return 1;
}

extern "C" void __cxa_guard_release(uint64_t *guard) noexcept {
  *guard |= 0x01;
}

extern "C" void __cxa_guard_abort(uint64_t *guard) noexcept {
}
} // namespace __cxxabiv1
