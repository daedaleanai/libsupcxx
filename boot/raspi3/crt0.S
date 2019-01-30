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

//------------------------------------------------------------------------------
// The stack - grows downwards on Aarch64 and needs to be 16 bytes aligned
//------------------------------------------------------------------------------
.section .bss
.align 16
stackBottom:
.skip 131072 // 128 KiB
stackTop:

.section .rodata

.global _initIo
.global bootInfo

.section .boot
.global _start
.type _start, @function

_start:
  mrs x1, mpidr_el1 // Mrs moves contents of a coprocesor register to an ARM
                    // register. Move the contents of the Multiprocessor
                    // Affinity Register to x1 (Cortex-A53 spec section 4.3.2)
  and x1, x1, #0x3  // x1 = x1 & 0x3 - the first two bits indicate the current
                    // core id
  cbz x1, run_el2   // Don't get into the infinit loop below if we're running
                    // on core #0
loop:
  wfe               // Wait for event - an interrupt or whatever
  b loop            // Handled the interrupt? Fine. Wait for another one.

//------------------------------------------------------------------------------
// Hypervisor mode (EL2)
//------------------------------------------------------------------------------
run_el2:
  // Set up the stack for EL1
  ldr x1, =stackTop
  msr sp_el1, x1

  // Provides the values of the virtualization versions of the processor id
  // registers. These are the value returned by non-secure EL1 reads of of
  // midr_el1 and mpidr_el1.
  mrs x0, midr_el1
  mrs x1, mpidr_el1
  msr vpidr_el2, x0
  msr vmpidr_el2, x1

  // Disable trapping to hypervisor for accesses to CPACR, Trace functionality
  // and registers associated with Advanced SIMD and Floating-point execution.
  mov x0, #0x33ff
  msr cptr_el2, x0

  // Don't trap floating point to EL1 from EL0 or EL1
  mov x0, #3 << 20
  msr cpacr_el1, x0

  // Make EL1 AArch64
  mov x0, #(1 << 31)
  msr hcr_el2, x0

  // Set up the system for EL1 - Res1 bits to 1, res0 bits to 0 and all other
  // bits to 0. Movk - move 16-bit immediate into register, keeping other bits
  // unchanged, do logical shift left (lsl).
  mov x0, #0x0800
  movk x0, #0x30d0, lsl #16
  msr sctlr_el1, x0

  // Set up the saved program status register, these will get set up as the
  // program status when the exception returns. We take the stack pointer from
  // sp_el1.
  mov x0, #0x5
  msr spsr_el2, x0

  // Where to jump on return
  ldr x0, =run_el1
  msr elr_el2, x0
  eret

//------------------------------------------------------------------------------
// OS Mode (EL1)
//------------------------------------------------------------------------------
run_el1:
  // Clear the frame pointer and the link register (eor == xor)
  eor x30, x30, x30
  eor x29, x29, x29

  // Clear bss
  ldr x1, =__bssStart
  ldr x2, =__bssEnd
bssLoopStart:
  cmp x1, x2
  b.ge bssLoopEnd
  str xzr, [x1], #8
  b bssLoopStart
bssLoopEnd:

  bl _initIo

  ldr x0, =bootInfo
  bl _systemSetup

  bl _init

  ldr x1, =bootInfo
  ldr x0, [x1]
  bl main

  bl _fini

  b loop
