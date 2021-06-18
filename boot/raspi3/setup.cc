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

#include <cstring>

// These variables do not exist, but their addresses are known to the linker.
// See the linker script for details.
extern uint32_t __kernelEnd;
extern uint32_t __initArrayBegin;
extern uint32_t __initArrayEnd;
extern uint32_t __finiArrayBegin;
extern uint32_t __finiArrayEnd;

BootInfo bootInfo;

// Page tables - we will only address the first 1GB using 2MB pages
uint64_t tableL1[512] [[gnu::aligned(4096)]];
uint64_t tableL2[512] [[gnu::aligned(4096)]];

// Table entry flags
// See: https://static.docs.arm.com/100940/0100/armv8_a_address%20translation_100940_0100_en.pdf
// Section 5.
#define MMU_TABLE 0b11        // Pointer to a page table of the next level
#define MMU_PAGE  0b01        // Pointer to a page
#define MMU_AF    (1<<10)     // Accessed flag
#define MMU_OSH   (2<<8)      // Outter Shareable
#define MMU_ISH   (3<<8)      // Inner Shareable
#define MMU_RAM   (0<<2)      // Regular RAM
#define MMU_DEV   (1<<2)      // Memory-mapped device

extern "C" void _systemSetup(BootInfo *info) {
  const char *cmdline = (const char*)0x12c;

  // Defaults for when the bootloader did not specify anything. This likely
  // means we are in qemu. In either case, the heap memory starts wherever the
  // kernel memory ends.
  info->heapStart = (uint64_t)&__kernelEnd;
  info->heapEnd = 0x3ec00000;

  // Figured that by poking at the memory with gdb, it does not seem to be
  // documented anywhere, but it works consistently, so here we are.
  info->cmdline = (char*)0x12c;

  // See if we have some bootloader information passed on the commandline
  const char *vcBase = strstr(cmdline, "vc_mem.mem_base=");
  if (vcBase) {
    // The bootloader inserts two space between the boot data and the
    // user-supplied commandline
    const char *userCmdLine = strstr(cmdline, "  ");
    if (userCmdLine) {
      info->cmdline = userCmdLine + 2;
    } else {
      info->cmdline = "";
    }

    // Figure out where our memory ends - this is where the videocore memory
    // starts. At leas I thinks so. Could not find any docs.
    vcBase += strlen("vc_mem.mem_base="); // name
    const char *ptr = vcBase;
    uint64_t heapEnd = strtoul(vcBase, &ptr, 16);
    if (ptr != vcBase && *ptr == ' ') {
      info->heapEnd = heapEnd;
    }
  }

  // Align the heap start to 16 bytes and heap end to 2MB because we later
  // map from the beginning of RAM in 2MB increments
  info->heapStart = (((info->heapStart-1)>>4)<<4)+16;
  info->heapEnd = (((info->heapEnd-1)>>21)<<21);

  // We need to have a possibility of using unaligned memory access. This is a
  // default on ARMv8 for "normal memory", however by default all memory is
  // treated as a device (MMIO) memory and, as such, can only be accessed in
  // aligned fasion. We need to enable MMU to tell the CPU what is a normal RAM
  // and what is a memory mapped device.
  // http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.faqs/ka15414.html
  // https://static.docs.arm.com/100940/0100/armv8_a_address%20translation_100940_0100_en.pdf

  // Zero out the page tables
  for (int i = 0; i < 512; ++i) {
    tableL1[i] = 0;
    tableL2[i] = 0;
  }

  // Point the first 1GB to the next level table because we have different kinds
  // of memory there
  uint64_t l2Addr = (uint64_t)tableL2;
  tableL1[0] = l2Addr    | // Address of the level2 table
               MMU_TABLE | // Expect a table at this address
               MMU_AF;     // The entry is used

  // We map normal memory from zero to wherever the heap end is located in 2MB
  // increments
  // See: https://static.docs.arm.com/100940/0100/armv8_a_address%20translation_100940_0100_en.pdf
  // Section 4.
  for (uint64_t addr = 0; addr < info->heapEnd; addr += 0x00200000) {
    uint64_t i = (addr >> 21) & 0x01ff; // Index of this virtual address
    tableL2[i] = addr     | // Physical address we want to use for this page
                 MMU_PAGE | // It's a page, 2MB at this level
                 MMU_AF   | // Accessed flag
                 MMU_ISH  | // Inner shareable
                 MMU_RAM;   // It's regular RAM (index 0 in mair_el1 below)
  }

  // The same as above except for the peripherals
  for (uint64_t addr = 0x3f000000; addr < 0x40000000; addr += 0x00200000) {
    uint64_t i = (addr >> 21) & 0x01ff; // Index of this virtual address
    tableL2[i] = addr     | // Physical address we want to use for this page
                 MMU_PAGE | // It's a 2M page at this level
                 MMU_AF   | // Accessed flag
                 MMU_OSH  | // Outer shareable
                 MMU_DEV;   // It's a memory-mapped device (index 1 in mair_el1
                            // below)
  }

  // Set up the Memory Attributes register
  // On device memory:
  // http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.den0024a/CHDBDIDF.html
  uint64_t reg = (0xff << 0) | // 0: normal memory, outer and inner write-back,
                               //    non-transient
                 (0x04 << 8);  // 1: device memory, nGnRE

  asm volatile(
    "msr mair_el1, %0"
    :
    : "r" (reg));

  // Set up the Translation Control Register
  // http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.ddi0500e/CIHFFGFG.html
  reg = (0b001ull << 32) | // (IPS) Intermediate physical size of 36-bits (64GB)
        (0b1ull   << 23) | // (EPD1) We don't use the higher part (TTBR1) so
                           // generate translation faults without looking for
                           // tables
        (0b00ull  << 14) | // (TG0) Translation granule for TTBR0_EL1 = 4k
        (0b11ull  << 12) | // (SH0) Sharability of the memory associated with
                           // translation table walks for TTBR0_EL1 = 3 (Inner
                           // Shareable)
        (0b01ull  << 10) | // (ORGN0) Outer cacheability = 1 (Write Back)
        (0b01ull  << 8)  | // (IRGN0) Inner cacheability = 1 (Write Back)
        (0b0ull   << 7)  | // (EPD0) Walk translation tables in TTBR0_EL1
        (25ull    << 0);   // (T0SZ) Size offset of the memory region addressed
                           // by TTBR0_EL1 = 25 (3 levels (512G))
  asm volatile(
    "msr tcr_el1, %0"
    :
    : "r" (reg));

  // Tell the MMU where the translation tables are and enable the MMU by setting
  // the M bit in the System Control Register
  asm volatile(
    "msr ttbr0_el1, %0\n\t"
    "isb\n\t"               // Instruction Synchronization Barrier
    "mrs x0, sctlr_el1\n\t"
    "orr x0, x0, #1\n\t"
    "msr sctlr_el1, x0\n\t"
    "isb\n\t"
    :
    : "r" ((uint64_t)tableL1));
}

// See: C++ Application Binary Interface Standard for the ARM 64-bit
// Architecture chapter 3.2.3. Also see the kernel.ld linker script.
extern "C" void _init() {
  typedef void (* const CtorPtr)();
  for (CtorPtr *ctor = (CtorPtr*)&__initArrayBegin;
       ctor != (CtorPtr*)&__initArrayEnd;
       ++ctor) {
    (*ctor)();
  }
}

extern "C" void _fini() {
  typedef void (* const DtorPtr)();
  for (DtorPtr *dtor = (DtorPtr*)&__finiArrayBegin;
       dtor != (DtorPtr*)&__finiArrayEnd;
       ++dtor) {
    (*dtor)();
  }
}
