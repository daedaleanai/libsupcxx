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

#include "printf.hh"
#include "vga.hh"
#include "string.hh"

#include <stdarg.h>

namespace {
const char digits[] = "0123456789abcdef";
uint32_t printNumStr(char *str, uint64_t num, int base) {
  if(base <= 0 || base > 16) {
    return 0;
  }

  if(num == 0) {
    str[0] = '0';
    return 1;
  }

  uint64_t n = num;
  uint32_t len = 0;
  char *cursor1 = str;
  char *cursor2 = str;
  while(n && cursor1 != str+31) {
    int rem = n % base;
    *cursor1 = digits[rem];
    n /= base;
    ++cursor1;
    ++len;
  }

  *cursor1 = 0;
  --cursor1;
  while(cursor1 > cursor2) {
    char tmp = *cursor1;
    *cursor1 = *cursor2;
    *cursor2 = tmp;
    --cursor1;
    ++cursor2;
  }

  return len;
}

int32_t printNum(uint64_t num, int base, int padZeros = 0) {
  char buffer[64];
  uint32_t len = printNumStr(buffer, num, base);
  int pad = 0;
  if(padZeros) {
    for(pad = 0; pad < padZeros-len; ++pad) {
      io::vgaTerminalPutChar('0');
    }
  }
  io::vgaTerminalWrite(buffer, len);
  return len + pad;
}

int32_t printNumS(int64_t num) {
  char buffer[65];
  char *ptr = buffer;
  uint32_t len = 0;

  if(num < 0) {
    *ptr = '-';
    ++ptr;
    num = -num;
    ++len;
  }
  len += printNumStr(ptr, num, 10);
  io::vgaTerminalWrite(buffer, len);
  return len;
}

uint32_t printFloat(double num) {
  // Normalize the number
  int32_t exponent = 0;
  double numAbs = num;
  if (num < 0) {
    numAbs = -num;
  }
  if(numAbs) {
    while(numAbs > 999999999.0) {
      ++exponent;
      numAbs /= 10.0;
    }
    while(numAbs < 0.000000001) {
      --exponent;
      numAbs *= 10;
    }
  }

  // Print
  int32_t integer = (int32_t)numAbs;
  int32_t len = 0;
  if(num < 0) {
    len = printNumS(-integer);
  } else {
    len = printNumS(integer);
  }

  int32_t fraction = (int32_t)((numAbs-integer) * 1000000000.0);
  if(fraction && 12-len > 1) {
    char buffer[10];
    buffer[0] = '.';
    for(int i = 1; i < 10; ++i) {
      buffer[i] = '0';
    }
    for(int i = 9; fraction && i > 0; --i) {
      int rem = fraction % 10;
      buffer[i] = digits[rem];
      fraction /= 10;
    }
    int i = 9;
    for(; buffer[i] == '0'; --i) {
      buffer[i] = 0;
    }
    ++i;
    if(12-len < i) {
      i = 12-len;
    }
    len += i;
    io::vgaTerminalWrite(buffer, i);
  }

  if(exponent) {
    io::vgaTerminalWrite("e", 1);
    len += printNumS(exponent);
  }
  return len;
}
} // namespace

namespace io {
int32_t printf(const char *format, ...) {
  va_list ap;
  int length  = 0;
  int sz      = 0;
  int base    = 0;
  int sgn     = 0;
  int fl      = 0;
  int ret     = 0;
  int written = 0;
  const char *cursor = format;
  const char *start  = format;

  va_start(ap, format);
  while(*cursor) {
    if(*cursor == '%') {
      vgaTerminalWrite(start, length);
      written += length;
      ++cursor;
      if(*cursor == 0) {
        break;
      }

      if(*cursor == 's') {
        const char *str = va_arg(ap, const char*);
        size_t slen = strlen(str);
        vgaTerminalWrite(str, slen);
        written += slen;
      } else if(*cursor == 'p') {
        uint64_t num = (uint64_t)va_arg(ap, int*);
        vgaTerminalWrite("0x", 2);
        ret = printNum(num, 16, 2*sizeof(int*));
        written += ret + 2;
      } else {
        while(*cursor == 'l') {
          ++sz;
          ++cursor;
        }
        if(sz > 2) {
          sz = 2;
        }

        if(*cursor == 'x') {
          base = 16;
        } else if(*cursor == 'u') {
          base = 10;
        } else if(*cursor == 'o') {
          base = 8;
        } else if(*cursor == 'd') {
          sgn  = 1;
        } else if(*cursor == 'f') {
          fl   = 1;
        }

        if(fl) {
          double num;
          if(sz <= 1) {
            num = va_arg(ap, double);
          } else {
            num = va_arg(ap, long double);
          }
          ret = printFloat(num);
          written += ret;
        }
        else if(!sgn) {
          uint64_t num;
          if(sz == 0) {
            num = va_arg(ap, unsigned);
          } else if(sz == 1) {
            num = va_arg(ap, unsigned long);
          } else {
            num = va_arg(ap, unsigned long long);
          }
          ret = printNum(num, base);
          written += ret;
        }
        else {
          int64_t num;
          if(sz == 0) {
            num = va_arg(ap, int);
          } else if(sz == 1) {
            num = va_arg(ap, long);
          } else {
            num = va_arg(ap, long long);
          }
          ret = printNumS(num);
          written += ret;
        }
        sz = 0; base = 0; sgn = 0;
      }
      ++cursor;
      start = cursor;
      length = 0;
      continue;
    }
    ++length;
    ++cursor;
  }
  if(length) {
    vgaTerminalWrite(start, length);
    written += length;
  }
  va_end (ap);
  return written;
}
} // namespace io
