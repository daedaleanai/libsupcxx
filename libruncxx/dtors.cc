//------------------------------------------------------------------------------
// Copyright (C) 2018-2019 Daedalean AG
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.
//
// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files LICENSE and LICENSE.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.
//------------------------------------------------------------------------------

#include <cstddef>

namespace {
  // Maximum number of global destructors that the runtime can keep on the
  // record. There's no heap memory at this point, and we want to keep things
  // simple, so we're stuck with whatever we preconfigure here.
  const size_t GLIBCXX_NUM_GLOBAL_DTORS = 100;
}

namespace {
struct DtorEntry {
  void (*dtor)(void*);
  void *arg;
};

size_t numDtors = 0;
DtorEntry dtors[GLIBCXX_NUM_GLOBAL_DTORS];
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
[[gnu::destructor]] void call_cxa_finalize() {
  __cxxabiv1::__cxa_finalize(nullptr);
}
} // namespace

} // extern "C"
} // namespace __cxxabiv1
