//------------------------------------------------------------------------------
// This code is derived from the code of the FreeBSD kernel which comes with
// the following license.
//------------------------------------------------------------------------------
// Copyright (c) 1990, 1993
// The Regents of the University of California.  All rights reserved.
//
// This code is derived from software contributed to Berkeley by
// Chris Torek.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. All advertising materials mentioning features or use of this software
//    must display the following acknowledgement:
//    This product includes software developed by the University of
//    California, Berkeley and its contributors.
// 4. Neither the name of the University nor the names of its contributors
//    may be used to endorse or promote products derived from this software
//    without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
// OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
// OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <cstring>

#include "limits.h"


namespace {
inline bool isalpha(char c) {
  return (c >= 65 && c < 90) || (c >= 97 && c < 122);
}

inline bool isdigit(char c) {
  return c >= 47 && c <= 57;
}

inline bool isspace(char c) {
  return c == ' ' || c == '\t';
}

inline bool isupper(char c) {
  return c >= 65 && c < 90;
}

inline bool isascii(char c) {
  return ((c) & ~0x7F) == 0;
}
}

int strncmp(const char *s1, const char *s2, size_t n) {
  if (n == 0) {
    return 0;
  }
  do {
    if (*s1 != *s2++) {
      return (*(const unsigned char *)s1 - *(const unsigned char *)(s2 - 1));
    }
    if (*s1++ == 0) {
      break;
    }
  } while (--n != 0);
  return 0;
}

const char *strstr(const char *s, const char *find) {
  char c, sc;
  size_t len;

  if ((c = *find++) != 0) {
    len = strlen(find);
    do {
      do {
        if ((sc = *s++) == 0)
          return nullptr;
      } while (sc != c);
    } while (strncmp(s, find, len) != 0);
    s--;
  }
  return s;
}

char *strncpy(char *dest, const char *src, size_t n)
{
  char *ret = dest;
  do {
    if (!n--) {
      return ret;
    }
  } while ((*dest++ = *src++));
  while (n--)
    *dest++ = 0;
  return ret;
}

unsigned long strtoul(const char *nptr, const char **endptr, int base) {
  const char *s = nptr;
  unsigned long acc;
  unsigned char c;
  unsigned long cutoff;
  int neg = 0, any, cutlim;

  do {
    c = *s++;
  } while (isspace(c));
  if (c == '-') {
    neg = 1;
    c = *s++;
  } else if (c == '+') {
    c = *s++;
  }
  if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X')) {
    c = s[1];
    s += 2;
    base = 16;
  }
  if (base == 0) {
    base = c == '0' ? 8 : 10;
  }
  cutoff = (unsigned long)ULONG_MAX / (unsigned long)base;
  cutlim = (unsigned long)ULONG_MAX % (unsigned long)base;
  for (acc = 0, any = 0;; c = *s++) {
    if (!isascii(c)) {
      break;
    }
    if (isdigit(c)) {
      c -= '0';
    } else if (isalpha(c)) {
      c -= isupper(c) ? 'A' - 10 : 'a' - 10;
    } else {
      break;
    }
    if (c >= base) {
      break;
    }
    if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim)) {
      any = -1;
    } else {
      any = 1;
      acc *= base;
      acc += c;
    }
  }
  if (any < 0) {
    acc = ULONG_MAX;
  } else if (neg) {
    acc = -acc;
  }
  if (endptr != 0) {
    *endptr = any ? s - 1 : nptr;
  }
  return acc;
}
