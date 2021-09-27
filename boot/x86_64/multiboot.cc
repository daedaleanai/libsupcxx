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

#include "libsupcxx/boot/bootinfo.hh"
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

extern BootInfo bootInfo;

// This variable does not exist, but it's address is known to the linker. See
// the linker script for details.
extern uint32_t __kernelEnd;

// We know what we're doing, don't warn please
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"

// Populate bootInfo from MbInfo
extern "C" void _parseMultiboot(uint32_t magic, MbInfo *info) {
  // Populate cmdline
  bootInfo.cmdline = (char *)info->cmdline;

  // We only populate one entry in bootInfo.memoryMap with the largest
  // usable memory region we can find. Here we find it
  MbMmapEntry *mmap = (MbMmapEntry *)info->mmapAddr;
  bool hasLargest = false;
  uint64_t addr = 0;
  uint64_t len = 0;

  for (uint32_t i = 0; i < info->mmapLength; ++i) {
    if (mmap[i].addr < 0x1000) {
      continue;
    }

    if (mmap[i].type != MULTIBOOT_MEMORY_AVAILABLE) {
      continue;
      // TODO: non-available regions are DEVICES. 
      // We should add them as DEVICE MemoryRegions
    }

    if (!hasLargest || len < mmap[i].len) {
      hasLargest = true;
      addr = mmap[i].addr;
      len = mmap[i].len;
    }
  }

  if (!hasLargest) {
    bootInfo.numMemoryRegions = 0;
    return;
  }

  // setUpHeap will align any MemoryRegion's start to 4K, but we also
  // align the boundaries to 2MB on x86_64
  uint64_t memEnd = addr + len;

  uint8_t shift = 21;
  uint32_t add = 0x00200000;

  addr = (((addr-1)>>shift)<<shift)+add;

  if (addr >= memEnd) {
    bootInfo.numMemoryRegions = 0;
    return;
  }

  memEnd = (((memEnd-1)>>shift)<<shift);

  // Populate bootInfo with the largest memory region we found
  bootInfo.numMemoryRegions = 1;
  bootInfo.memoryMap[0].type = MemoryType::RAM;
  bootInfo.memoryMap[0].start = addr;
  bootInfo.memoryMap[0].end = memEnd;
}
// You can warn again
#pragma GCC diagnostic push
