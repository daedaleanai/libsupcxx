//------------------------------------------------------------------------------
// Copyright (C) 2019 Daedalean AG
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
#include <cstdint>
#include <cstddef>

namespace {
// We may need these PDTs if the heap is not aligned to 1GB boundaries, which
// is most likely the case
uint64_t pdt1[512] [[gnu::aligned(4096)]];
uint64_t pdt2[512] [[gnu::aligned(4096)]];

const uint64_t ADDR_MASK = 0xfffffffffffff000;
const uint64_t PAGE_FLAGS = 0x83; // PSE, present, and r/w
const uint64_t PDT_FLAGS = 0x3;   // present, and r/w
}

extern "C" void _systemSetup(BootInfo *info) {
  // Set up SSE2 - so that we don't fault when the compiler generates code
  // accessing the SSE registers. The AMD64 spec enforces SSE2 as a minimum
  // requirement for a compliant CPU, so gcc generates code using that by
  // default.
  asm volatile(
    "mov %cr0, %eax\n\t"
    "and $0xfffffffb, %eax\n\t" // Clear the FPU emulation bit - it should be
    "or $0x2, %eax\n\t"         // clear already, but we do it just to be sure.
    "mov %eax, %cr0\n\t"        // Also set the the co-processor monitoring -
                                // same as above - should be set already, but we
                                // do it anyways.
    "mov %cr4, %eax\n\t"
    "or $0x00000600, %eax\n\t"  // enable SSE and SSE exceptions
    "mov %eax, %cr4\n\t");      // the OSFXSR and OSXMMEXCPT bits

  if (info->numMemoryRegions == 0 ||
      info->memoryMap[0].type != MemoryType::RAM) {
    return;
  }

  // For now we only support one RAM region in x86_64
  uint64_t heapStart = info->memoryMap[0].start;
  uint64_t heapEnd = info->memoryMap[0].end;

  // Identity map the heap if we have at least 2MB of it as that's the smallest
  // we can fit into a PAE+PSE page directory entry.
  if (heapStart == 0 || heapEnd - heapStart < 0x00200000) {
    info->numMemoryRegions = 0;
    return;
  }

  // We also don't look beyond the first 512GB
  if ((heapStart >> 39) > 0) {
    info->numMemoryRegions = 0;
    return;
  }

  // Zero out the two marginal page descriptor tables
  for (int i = 0; i < 512; ++i) {
    pdt1[i] = 0;
    pdt2[i] = 0;
  }

  // Get the address of the PDPT
  uint64_t pml4tEntry;
  uint64_t *pdpt = 0;
  asm volatile(
    "mov %%cr3, %%ebx\n\t"
    "xor %%rax, %%rax\n\t"
    "movl (%%ebx), %%eax\n\t"
    : "=a" (pml4tEntry));
  pml4tEntry &= ADDR_MASK;
  pdpt = (uint64_t *)pml4tEntry;

  // Check whether the fist page can be a huge page (1GB) - happens if the
  // virtual address is 1GB aligned. If not, allocate smaller pages, but still
  // PAE+PSE -> 2MB
  uint64_t addr = heapStart;
  uint64_t addrAlign = heapStart;
  addrAlign >>= 30;
  addrAlign <<= 30;

  uint16_t pdptIndex = (addr >> 30) & 0x1ff;
  uint16_t pdtIndex = (addr >> 21) & 0x1ff;
  if (addrAlign != addr) {
    uint64_t *pdt;
    // We don't need a PDT because we installed one already in the assembly code
    if (pdpt[pdptIndex] != 0) {
      uint64_t pdptEntry = pdpt[pdptIndex];
      pdptEntry &= ADDR_MASK;
      pdt = (uint64_t *)pdptEntry;
      // We need a pdt for this page
    } else {
      pdt = pdt1;
      uint64_t pdptEntry = (uint64_t)pdt;
      pdptEntry |= PDT_FLAGS;
      pdpt[pdptIndex] = pdptEntry;
    }

    for (; pdtIndex < 512; ++pdtIndex) {
      pdt[pdtIndex] = addr | PAGE_FLAGS;
      addr += 0x00200000;
    }
  }

  // Map huge pages as long as we can
  for (pdptIndex = (addr >> 30) & 0x1ff;
       addr + 0x40000000 <= heapEnd && pdptIndex < 512;
       addr += 0x40000000, ++pdptIndex) {
    pdpt[pdptIndex] = addr | PAGE_FLAGS;
  }

  // Check if we have anything else to map and, if so, install another PDT
  if (addr < heapEnd) {
    pdptIndex = (addr >> 30) & 0x1ff;
    uint64_t pdptEntry = (uint64_t)pdt2;
    pdptEntry |= PDT_FLAGS;
    pdpt[pdptIndex] = pdptEntry;
  }

  // Map the reminder as small PAE+PSE pages -> 2MB
  for (pdtIndex = 0; pdtIndex < 512 && addr < heapEnd; ++pdtIndex) {
    pdt2[pdtIndex] = addr | PAGE_FLAGS;
    addr += 0x00200000;
  }
}
