
namespace __cxxabiv1 {
  struct __cxa_exception;
  struct __cxa_eh_globals {
    __cxa_exception *caughtExceptions;
    unsigned int uncaughtExceptions;
  };
}

namespace {
  __cxxabiv1::__cxa_eh_globals globals;
}

namespace __cxxabiv1 {
extern "C" __cxa_eh_globals *__cxa_get_globals_fast() noexcept {
  return &globals;
}

extern "C" __cxa_eh_globals *__cxa_get_globals() noexcept {
  return &globals;
}
}
