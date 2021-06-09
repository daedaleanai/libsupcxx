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

#include "libsupcxx/libsupcxx/src/unwind-cxx.h"
#include <exception>
#include <stddef.h>
#include <stdint.h>

extern "C" void *malloc(size_t);
extern "C" void free(void *);

// C++ throws the std::bad_alloc exception when it fails to allocate memory.
// We need to keep all exception objects out of the stack because the stack
// gets unwound when processing an exception. We therefore stash some memory
// so that we always have enough to at least throw std::bad_alloc.
//
// See: https://itanium-cxx-abi.github.io/cxx-abi/abi-eh.html#cxx-allocate
namespace {
const uint16_t OBJ_SIZE = 1024;
const uint16_t NUM_OBJS = 8;

char emergencyPool[NUM_OBJS][OBJ_SIZE];
bool emergencyAllocated[NUM_OBJS] = {false};
} // namespace

namespace __cxxabiv1 {
extern "C" void *__cxa_allocate_exception(size_t thrownSize) noexcept {
  // The ABI requires us to have a preamble for all the exception metadata
  thrownSize += sizeof(__cxa_refcounted_exception);
  void *ret = malloc(thrownSize);

  if (!ret && thrownSize <= OBJ_SIZE) {
    for (uint32_t i = 0; i < NUM_OBJS; ++i) {
      if (!emergencyAllocated[i]) {
        emergencyAllocated[i] = true;
        ret = (void*)emergencyPool[i];
        break;
      }
    }
  }

  if (!ret) {
    std::terminate();
  }
  // The preamble is one thing, but we need to return the pointer to the actual
  // exception.
  return (void*)((char*)ret + sizeof(__cxa_refcounted_exception));
}

extern "C" void __cxa_free_exception(void *vptr) noexcept {
  // We allocated the object with the preamble, so we need to adjust the pointer
  char *ptr = (char*)vptr - sizeof(__cxa_refcounted_exception);
  bool freed = false;
  for (uint32_t i = 0; i < NUM_OBJS; ++i) {
    if (emergencyPool[i] == ptr) {
      emergencyAllocated[i] = 0;
      freed = true;
    }
  }

  if (!freed) {
    free(ptr);
  }
}

} // namespace __cxxabiv1
