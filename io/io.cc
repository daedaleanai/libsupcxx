//------------------------------------------------------------------------------
// Copyright (C) 2016 Lukasz Janyst <lukasz@jany.st>
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

#include "io.hh"

extern "C" void _putChar(char c);

namespace io {
void putString(const char *str) {
  if (!str) {
    return;
  }
  for (; *str; ++str) {
    _putChar(*str);
  }
}

void putString(const char *str, size_t size) {
  if (!str) {
    return;
  }
  for (size_t i = 0; i < size; ++i) {
    _putChar(str[i]);
  }
}

}
