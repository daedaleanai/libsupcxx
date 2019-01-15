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

#include "../bootinfo.hh"
#include <stdint.h>

#define MULTIBOOT_MEMORY_AVAILABLE 1

struct [[gnu::packed]] MbMmapEntry {
  uint32_t size;
  uint64_t addr;
  uint64_t len;
  uint32_t type;
};

struct MbInfo {
  uint32_t stuff1[4];
  uint32_t cmdline;
  uint32_t stuff2[6];
  uint32_t mmapLength;
  uint32_t mmapAddr;
};

BootInfo bootInfo;

// This variable does not exist, but it's address is known to the linker. See
// the linker script for details.
extern uint32_t __kernelEnd;

// We know what we're doing, don't warn please
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"

extern "C" void _parseMultiboot(uint32_t magic, MbInfo *info) {
  // Find the largest available memory region for the heap
  MbMmapEntry *mmap = (MbMmapEntry *)info->mmapAddr;
  bool hasLargest = false;
  uint64_t addr = 0;
  uint64_t len = 0;

  for (uint32_t i = 0; i < info->mmapLength; ++i) {
    if(mmap[i].type != MULTIBOOT_MEMORY_AVAILABLE) {
      continue;
    }

    // 32-bits
    if (sizeof(int*) == 4 && mmap[i].addr >= 0xffffffff) {
      continue;
    }

    uint64_t trimmedLen = mmap[i].len;
    if (sizeof(int*) == 4 && mmap[i].addr + mmap[i].len > 0xffffffff) {
      trimmedLen = 0xffffffff - mmap[i].addr;
    }

    if (!hasLargest || len < trimmedLen) {
      hasLargest = true;
      addr = mmap[i].addr;
      len = mmap[i].len;
    }
  }

  // Set the boot info structure
  bootInfo.cmdline = (char *)info->cmdline;
  bootInfo.heapStart = 0;
  bootInfo.heapEnd = 0;

  if (!hasLargest) {
    return;
  }

  // See if the largest regions happens to overlap with where the kernel is
  // loaded.
  uint64_t kernelEnd = (uint64_t)&__kernelEnd;
  uint64_t memEnd = addr+len;
  if (kernelEnd > addr && kernelEnd <= memEnd) {
    addr = kernelEnd;
  }

  // Align the boundaries to 4MB on x86 and to 2MB on x86_64
  uint8_t shift = 22;
  uint32_t add = 0x00400000;

  if(sizeof(int*) == 8) {
    uint8_t shift = 21;
    uint32_t add = 0x00200000;
  }

  addr = (((addr-1)>>shift)<<shift)+add;

  if (addr >= memEnd) {
    return;
  }

  memEnd = (((memEnd-1)>>shift)<<shift);

  bootInfo.heapStart = addr;
  bootInfo.heapEnd = memEnd;
}
// You can warn again
#pragma GCC diagnostic push
