
#include "../bootinfo.hh"
#include <io/printf.hh>
#include <stdint.h>

#define MULTIBOOT_MEMORY_AVAILABLE 1

struct MbMmapEntry {
  uint32_t size;
  uint64_t addr;
  uint64_t len;
  uint32_t type;
} __attribute__((packed));

struct MbInfo {
  uint32_t stuff1[4];
  uint32_t cmdline;
  uint32_t stuff2[6];
  uint32_t mmapLength;
  MbMmapEntry *mmap;
};

BootInfo bootInfo;

// This variable does not exist, but it's address is known to the linker. See
// the linker script for details.
extern uint32_t __kernelEnd;

extern "C" void _parseMultiboot(uint32_t magic, MbInfo *info) {
  // Find the largest available memory region for the heap
  MbMmapEntry *mmap = info->mmap;
  bool hasLargest = false;
  uint64_t addr = 0;
  uint64_t len = 0;
  for (uint32_t i = 0; i < info->mmapLength; ++i) {
    if(mmap[i].type != MULTIBOOT_MEMORY_AVAILABLE) {
      continue;
    }
    if (!hasLargest || len < mmap[i].len) {
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

  // Align the boundary to 64 bits
  addr = (((addr-1)>>3)<<3)+8;

  if (addr >= memEnd) {
    return;
  }

  bootInfo.heapStart = addr;
  bootInfo.heapEnd = memEnd;
}
