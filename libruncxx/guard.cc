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

#include <stdint.h>
#include "libsupcxx/libsupcxx/src/exception_defines.h"
#include "libsupcxx/libsupcxx/src/cxxabi.h"

#if __cpp_exceptions
namespace __gnu_cxx {
  recursive_init_error::~recursive_init_error() noexcept { }
}
#endif

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
