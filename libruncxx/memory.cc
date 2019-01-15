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

#include <boot/bootinfo.hh>

#include <stdint.h>
#include <stddef.h>

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
  if (head == 0 || size > 0x7fffffff) {
    return nullptr;
  }

  // Allocating anything less than 8 bytes is kind of pointless, the
  // book-keeping overhead is too big. We will also align to 4 bytes.
  uint32_t allocSize = (((size-1)>>2)<<2)+4;
  if (allocSize < 8) {
    allocSize = 8;
  }

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

extern BootInfo bootInfo;

namespace {
[[gnu::constructor]] void setUpHeap() {
  head = (MemChunk*)bootInfo.heapStart;
  head->next = 0;
  head->size = bootInfo.heapEnd - bootInfo.heapStart - sizeof(MemChunk);
}
}
