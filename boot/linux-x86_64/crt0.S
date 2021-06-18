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
// This will be run in a hosted, but freestanding, Linux environment
//------------------------------------------------------------------------------

#include "libsupcxx/boot/linux-x86_64/syscall.h"
.section .text

.global _start
.type _start, @function

.global _syscall
.type _syscall, @function

_start:
  // We need to get argc, argv, and envp from the stack for _systemSetup.
  // Linux gives us the stack as: 
  // ($rsp) argc -- argv[0] -- argv[1] -- ... -- argv[argc - 1] -- envp
  mov (%rsp), %rdi             // put argc in rdi
  lea 8(%rsp), %rsi            // put argv in rsi
  lea 8(%rsi, %rdi, 8), %rdx   // put envp in rdx

  // Set up end of the stack frame linked list - gdb and exception handling
  // really like it ;)
  mov $0, %rbp
  pushq %rbp  // rip=0
  pushq %rbp  // ebp=0
  mov %rsp, %rbp

  // Call _systemSetup(envp) to initialise the heap and populate bootInfo
  call _systemSetup

  // Run the global constructors
  call _init

  // Get the commandline string pointer and start main
  mov bootInfo, %rdi
  call main

  // Run the global destructors
  call _fini

  // exit(0) (main does not return anything)
  mov $0, %rdi
  mov $SYS_exit, %rax
  syscall              // syscall and exit. no return from this
  hlt                  // if we return from exit(), something has gone wrong


// C-callable syscalls.
// Usage: _syscall(<syscall_number>, [syscall_args])
_syscall:
  // Convert C varargs to syscall argument format
  movq %rdi, %rax
  movq %rsi, %rdi
  movq %rdx, %rsi
  movq %rcx, %rdx
  movq %r8, %r10
  movq %r9, %r8
  movq 8(%rsp), %r9

  syscall
  retq
