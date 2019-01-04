
#include <boot/bootinfo.hh>

#include <stdint.h>
#include <stddef.h>

namespace {
struct MemChunk {
  MemChunk *next;
  uint32_t  size;
};

static MemChunk *head = 0;
const uint32_t MEMCHUNK_USED = 0x80000000;
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
