
#include "config.hh"

namespace {
struct DtorEntry {
  void (*dtor)(void*);
  void *arg;
};

size_t numDtors = 0;
DtorEntry dtors[__cxxabiv1::GLIBCXX_NUM_GLOBAL_DTORS];
} // namespace

namespace __cxxabiv1 {
extern "C" {

int __cxa_atexit(void (*dtor)(void*), void *arg, void *dsoHandle) noexcept {
  if (numDtors == GLIBCXX_NUM_GLOBAL_DTORS) {
    return -1;
  }

  dtors[numDtors].dtor = dtor;
  dtors[numDtors].arg = arg;
  // this is a kernel, we don't care about any DSO contex - ignore dsoHandle
  ++numDtors;
  return 0;
}

int __cxa_finalize(void *dtor) {
  // If we get a nullptr here, we call all of the destructors
  if (dtor == nullptr) {
    for (int i = numDtors - 1; i >= 0; --i) {
      dtors[i].dtor(dtors[i].arg);
    }
    numDtors = 0;
    return 0;
  }

  // If we got a pointer we search for the destructor and call it if found
  for (size_t i = numDtors - 1; i >= 0; --i) {
    if (dtors[i].dtor == dtor) {
      dtors[i].dtor(dtors[i].arg);
      dtors[i].dtor = nullptr;
    }
  }

  // Elliminate all the nullptrs set in the previous loop
  size_t j = 0;
  for (size_t i = 0; i < numDtors; ++i) {
    if (dtors[i].dtor == nullptr) {
      continue;
    }
    dtors[j].dtor = dtors[i].dtor;
    dtors[j].arg = dtors[i].arg;
    ++j;
  }
  numDtors = j;

  return 0;
}

namespace {
// register the destructor with the runtime
__attribute__((destructor)) void call_cxa_finalize() {
  __cxxabiv1::__cxa_finalize(nullptr);
}
} // namespace

} // extern "C"
} // namespace __cxxabiv1
