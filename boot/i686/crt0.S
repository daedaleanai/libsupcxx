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
// The multiboot standard does not define the value of the stack pointer and it
// is up to us to provide a stack. This allocates room for a small stack by
// creating a symbol at the bottom of it, then allocating some space, and
// finally creating a symbol at the top. The stack grows downwards on x86. The
// stack is in its own section so it can be marked nobits, which means the
// kernel file does not contain an uninitialized stack. The stack on x86 must
// be 16-byte aligned according to the System V ABI standard and de-facto
// extensions. The compiler will assume the stack is properly aligned and
// failure to align the stack will result in undefined behavior.
//------------------------------------------------------------------------------
.section .bss
.align 16
stack_bottom:
.skip 65536 // 64 KiB
stack_top:

.global bootInfo

//------------------------------------------------------------------------------
// The entry point to the kernel
//------------------------------------------------------------------------------
.section .text
.global _start
.type _start, @function
_start:
  // Set up the stack
  mov $stack_top, %esp

  // Set up end of the stack frame linked list - gdb really likes it ;)
  mov $0, %ebp
  push %ebp // rip=0
  push %ebp // ebp=0
  movl %esp, %ebp

  // Load the multiboot parameters on the stack and call the parser
  push %ebx
  push %eax
  call _parseMultiboot
  add $8, %esp

  // Set up the protected mode and page tables
  mov $bootInfo, %eax
  push %eax
  call _systemSetup
  add $4, %esp

  // Run the global constructors
  call _init

  // Get the commandline string pointer and start main
  mov bootInfo, %eax
  push %eax
  call main
  add $4, %esp

  // Tun the global destructors
  call _fini

  // Halt - probably makes sense to turn off the box here
  cli
  hlt
.size _start, . - _start
