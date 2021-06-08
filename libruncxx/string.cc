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

#include <stddef.h>

extern "C" size_t strlen(const char *str) {
  size_t len;
  for (len = 0; *str; ++str, ++len) {};
  return len;
}

extern "C" void *memcpy(void *dest, const void *src, size_t n) {
  char *d = (char*)dest;
  const char *s = (const char*)src;
  for (size_t i = 0; i < n; ++i) {
    *d++ = *s++;
  }
  return dest;
}

extern "C" void *memset(void *s, int c, size_t n) {
  char *data = (char*)s;
  for (size_t i = 0; i < n; ++i) {
    data[i] = c;
  }
  return s;
}

extern "C" char *strchr(const char *s, int c) {
  do {
    if (*s == c) {
      return (char*)s;
    }
  } while (*s++);
  return 0; 
}
