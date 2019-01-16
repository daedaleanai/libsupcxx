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

//------------------------------------------------------------------------------
// A multiboot bootloader will put as in a 32 bit protected mode by default
// and we need to set up the long mode ourselves.
//------------------------------------------------------------------------------
.code32

//------------------------------------------------------------------------------
// Multiboot
//------------------------------------------------------------------------------
.set ALIGN,    1<<0             // align loaded modules on page boundaries
.set MEMINFO,  1<<1             // provide memory map
.set FLAGS,    ALIGN | MEMINFO
.set MAGIC,    0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS) // checksum of above

// The bootloader will look for this, so we need to place it close to the
// beginning of the file - see the linker script
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

//------------------------------------------------------------------------------
// The stack
//------------------------------------------------------------------------------
.section .bss
.align 16
stackBottom:
.skip 131072 // 128 KiB
stackTop:

//------------------------------------------------------------------------------
// Page level structures: Page Map Level 4 Table (PML4T), Page Directory Pointer
// Table (PDPT), and Page Directory Table (PDT). We use PSE, so we don't bother
// with the actual page tables and map either 1G (PDPT PSE) or 2M (PDT PSE) at
// a time.
//------------------------------------------------------------------------------
.align 4096
PML4T:
.skip 4094 // 4 KiB
.align 4096
PDPT:
.skip 4094 // 4 KiB
.align 4096
PDT:
.skip 4094 // 4 KiB

//------------------------------------------------------------------------------
// 64 bit global segment descriptor table - see section 4.8 of the AMD64
// Architecture Programmer’s Manual, Volume 2: System Programming
//------------------------------------------------------------------------------
.section .data
GDT64:
  // The null segment
  .quad 0

  // The code segment
  .long 0
  .word 0x9a00 // Present, user+code descriptor, readable
  .word 0x0020 // Long mode

  // The data segment
  .long 0
  .word 0x9200 // Present, user+data descriptor, writeable
  .word 0

// The GDT struct pointer
GDTPointer:
  .word GDTPointer - GDT64 - 1 // Size of the table
  .quad GDT64                  // Address of the table

//------------------------------------------------------------------------------
// We need to stash the multiboot parameters so that we can pass them to the
// parser funstion later
//------------------------------------------------------------------------------
MultibootMagic:
  .long 0
MultibootPtr:
  .long 0

//------------------------------------------------------------------------------
// The entry point to the kernel
//------------------------------------------------------------------------------
.section .text
.global _start
.type _start, @function
_start:
  // Set up the stack and store the multiboot data
  mov $stackTop, %esp
  mov %ebx, MultibootPtr
  mov %eax, MultibootMagic

  // Zero up the PML4T
  mov $PML4T, %edi    // put the PML4T address in edi
  xor %eax, %eax      // zero up eax
  mov $1024, %ecx     // number of assignments to ecx 4096/4 because we will be
                      // setting double words
  rep stosl           // memset PML4T to zeros (eax) using double words

  // Zero up the PDPT
  mov $PDPT, %edi     // put the PDPT address in edi
  mov $1024, %ecx     // number of assignments to ecx 4096/4 because we will be
                      // setting double words
  rep stosl           // memset PDPT to zeros (eax) using double words

  // Zero up the PDT
  mov $PDT, %edi      // put the PDT address in edi
  mov $1024, %ecx     // number of assignments to ecx 4096/4 because we will be
                      // setting double words
  rep stosl           // memset PDT to zeros (eax) using double words

  // Identity page the first 8M of physical memory - that should be enough for
  // the kernel code and data
  mov $PDPT, %ebx     // PDPT address to ebx
  mov $PML4T, %eax    // PML4T address to eax
  or $0x3, %ebx       // The address is present and r/w
  mov %ebx, (%eax)    // Store the entry in the PML4T

  mov $PDT, %ebx      // PDT address to ebx
  mov $PDPT, %eax     // PDPT address to eax
  or $0x3, %ebx       // The address is present and r/w
  mov %ebx, (%eax)    // Store the entry in the PDPT

  mov $PDT, %eax      // PDT address to eax
  mov $0x83, %ebx     // A page starting at 0, using PSE, present, and r/w
  mov %ebx, (%eax)    // Page entry to PDT[0]
  mov $0x200083, %ebx // A page starting at 2MiB, using PSE, present, and r/w
  mov %ebx,8(%eax)    // Page entry to PDT[1]
  mov $0x400083, %ebx // A page starting at 4MiB, using PSE, present, and r/w
  mov %ebx, 16(%eax)  // Page entry to PDT[2]
  mov $0x600083, %ebx // A page starting at 6MiB, using PSE, present, and r/w
  mov %ebx, 24(%eax)  // Page entry to PDT[3]

  mov $PML4T, %eax    // Page map level 4 table to eax
  mov %eax, %cr3      // and to cr3

  mov %cr4, %eax
  or $0x30, %eax      // Set up  PSE and PAE
  mov %eax, %cr4

  mov $0xc0000080, %ecx // Set the ecx to 0xC0000080, which is the EFER MSR
  rdmsr               // Read from the model-specific register
  or $0x100, %eax     // Set the LM-bit which is the 9th bit
  wrmsr               // Write the model specific register

  mov %cr0, %eax
  or $0x80000000, %eax // Enable paging 32nd bit (PG)
  mov %eax, %cr0

  lgdt GDTPointer      // Load the 64-bit global descriptor table
  ljmp $0x8, $_start64 // Set the code segment and enter 64-bit long mode

//------------------------------------------------------------------------------
// Alright, we can use 64 bit instructions from now on
//------------------------------------------------------------------------------
.code64

.section .text
.global _start64
.type _start64, @function
_start64:
  mov $0x10, %eax      // Data segment descriptor offset to eax
  mov %ax, %ds         // Set the data segment
  mov %ax, %es         // Set the extra segment
  mov %ax, %fs         // Set the f-segment
  mov %ax, %gs         // Set the g-segment
  mov %ax, %ss         // Set the stack segment

  // Set up end of the stack frame linked list - gdb and exception handling
  // really like it ;)
  mov $0, %rbp
  pushq %rbp  // rip=0
  pushq %rbp  // ebp=0
  mov %rsp, %rbp

  // Load the multiboot parameters on to the registers and call the parser
  xor %rdi, %rdi
  xor %rsi, %rsi
  movl MultibootMagic, %edi
  movl MultibootPtr, %esi
  call _parseMultiboot

  // Run the configuration necessary to set the platrofm up
  mov $bootInfo, %rdi
  call _systemSetup

  // Run the global constructors
  call _init

  // Get the commandline string pointer and start main
  mov bootInfo, %rdi
  call main

  // Run the global destructors
  call _fini

  // Halt - probably makes sense to turn off the box here
  cli
  hlt
