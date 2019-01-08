
#include "../bootinfo.hh"
#include <cstdint>
#include <cstddef>

uint32_t pageDirectory[1024] [[gnu::aligned(4096)]];

extern "C" void _systemSetup(BootInfo *info) {
  // Zero our the page directory
  for (size_t i = 0; i < 1024; ++i) {
    pageDirectory[i] = 0;
  }

  // Identity page the whole memory
  pageDirectory[0] = 0x83;
  for (uint32_t addr = (uint32_t)info->heapStart, i = 1;
       addr < (uint32_t)info->heapEnd && i < 1024;
       addr += 0x400000) {
    pageDirectory[i++] = addr | 0x83;
  }

  // Load the page directory into cr3
  asm volatile(
    "mov %%eax, %%cr3\n\t"
    :
    : "a" (&pageDirectory));

  // Enable PSE and paging
  asm volatile(
    "mov %cr4, %eax\n\t"
    "or $0x10, %eax\n\t"
    "mov %eax, %cr4\n\t"
    "mov %cr0, %eax\n\t"
    "or $0x80000000, %eax\n\t"
    "mov %eax, %cr0\n\t");
}
