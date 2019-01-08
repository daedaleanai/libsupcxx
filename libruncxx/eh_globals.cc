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
