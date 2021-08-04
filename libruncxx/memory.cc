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

#include "libsupcxx/io/memory.hh"
#include "libsupcxx/boot/bootinfo.hh"

#include <stdint.h>
#include <stddef.h>

// This variable does not exist, but its address is known to the linker. See the
// linker script for details.
extern uint32_t __kernelEnd;

namespace {
struct MemChunk {
  MemChunk *next;
  uint64_t  size;
};

static MemChunk *head = 0;
const uint64_t MEMCHUNK_USED = 0x8000000000000000;
}

extern "C" void *malloc(size_t size) {
  // We don't allocate chunks this big
  if (head == 0 || size > 0x7fffffffffffffff) {
    return nullptr;
  }

  // Allocating anything less than 8 bytes is kind of pointless, the
  // book-keeping overhead is too big. We will also align to 8 bytes.
  // AArch64 platforms refuse to cooperate otherwise.
  uint64_t allocSize = (((size-1)>>3)<<3)+8;

  // Try to find a suitable chunk that is unused
  MemChunk *chunk = head;
  while (chunk) {
    if (!(chunk->size & MEMCHUNK_USED) && chunk->size >= allocSize) {
      break;
    }
    chunk = chunk->next;
  }

  if (!chunk) {
    return nullptr;
  }

  // Split the chunk if it's big enough to contain one more header and enough
  // bytes to handle another chunk of size 8 bytes.
  if (chunk->size >= allocSize + sizeof(MemChunk) + 8) {
    MemChunk *newChunk = (MemChunk *)((char *)chunk + sizeof(MemChunk) + allocSize);
    newChunk->size = chunk->size - allocSize - sizeof(MemChunk);
    newChunk->next = chunk->next;
    chunk->next = newChunk;
    chunk->size = allocSize;
  }

  // Mark the chunk as used and return the memory
  chunk->size |= MEMCHUNK_USED;
  return (char*)chunk + sizeof(MemChunk);
}

extern "C" void free(void *ptr) {
  if (!ptr) {
    return;
  }

  MemChunk *chunk = (MemChunk *)((char *)ptr - sizeof(MemChunk));
  chunk->size &= ~MEMCHUNK_USED;
}

extern "C" void reconfigureHeap(io::Region regions[], unsigned int numRegions) {
  if (numRegions == 0) {
    head = 0;
    return;
  }

  for (uint32_t i = 0; i < numRegions; ++i) {
    MemChunk *chunk = (MemChunk *)regions[i].address;
    chunk->size = regions[i].size;
    if (i + 1 == numRegions) {
      chunk->next = nullptr;
    } else {
      chunk->next = (MemChunk *)regions[i + 1].address;
    }
  }

  head = (MemChunk *)regions[0].address;
}

extern BootInfo bootInfo;

namespace {
[[gnu::constructor]] void setUpHeap() {
  MemChunk fakeHead;
  MemChunk *cursor = &fakeHead;
  uint64_t kernelEnd = (uint64_t)&__kernelEnd;

  for (uint16_t i = 0; i < bootInfo.numMemoryRegions; ++i) {
    uint64_t start = bootInfo.memoryMap[i].start;
    uint64_t end = bootInfo.memoryMap[i].end;
    const MemoryType type = bootInfo.memoryMap[i].type;

    // We can only put the heap in RAM
    if (type != MemoryType::RAM) {
      continue;
    }

    uint64_t size = end - start;

    // Check if this RAM segment contains the kernel
    if (start < kernelEnd && end >= kernelEnd) {
      start = kernelEnd;
    }

    // Check if this RAM segment contains the kernel commandline. We assume that
    // it may be located immediately after the kernel or at the beginning of the
    // chunk. We also assume that the commandline fits the page.
    uint64_t cmdlineAddr = (uint64_t)bootInfo.cmdline;
    if (start <= cmdlineAddr && end > cmdlineAddr) {
      start = (uint64_t)bootInfo.cmdline + 0x1000;
    }

    // Align the start to 4K
    start -= 1;
    start >>= 12;
    start <<= 12;
    start += 0x1000;
    if (end >= start) {
      size = end - start;
    } else {
      size = 0;
    }

    // We only care if it's at least a Megabyte
    if (size < 0x00100000) {
      continue;
    }

    cursor->next = (MemChunk *)start;
    cursor = cursor->next;
    cursor->next = 0;
    cursor->size = size - sizeof(MemChunk);
  }
  head = fakeHead.next;
}
}
