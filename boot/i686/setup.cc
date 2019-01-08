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
