
#include "../libsupcxx/src/cxxabi.h"

extern "C" void exit() noexcept {
  __cxxabiv1::__cxa_finalize(nullptr);
  asm volatile ("hlt");
}

typedef void (*atExitFn)(void*);

extern "C" int atexit(void (*fn)()) noexcept {
  return __cxxabiv1::__cxa_atexit((atExitFn)fn, 0, 0);
}
