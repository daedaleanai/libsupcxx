/*------------------------------------------------------------------------------
 | Copyright (C) 2019 Daedalean AG
 |
 | This library is free software; you can redistribute it and/or modify it
 | under the terms of the GNU General Public License as published by the
 | Free Software Foundation; either version 3, or (at your option)
 | any later version.
 |
 | This library is distributed in the hope that it will be useful,
 | but WITHOUT ANY WARRANTY; without even the implied warranty of
 | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 | GNU General Public License for more details.
 |
 | Under Section 7 of GPL version 3, you are granted additional
 | permissions described in the GCC Runtime Library Exception, version
 | 3.1, as published by the Free Software Foundation.
 |
 | You should have received a copy of the GNU General Public License and
 | a copy of the GCC Runtime Library Exception along with this program;
 | see the files LICENSE and LICENSE.RUNTIME respectively.  If not, see
 | <http://www.gnu.org/licenses/>.
 *----------------------------------------------------------------------------*/

SECTIONS
{
  /*----------------------------------------------------------------------------
   | 512KB both VMA and LMA - that's where the bootloader will put us.
   *--------------------------------------------------------------------------*/
  . = 0x80000;

  /*----------------------------------------------------------------------------
   | The code
   *--------------------------------------------------------------------------*/
  .text :
  {
    SORT(*libboot_first.a:crt0*)(.boot)
    *(.text*)
    *(.gnu.linkonce.t*)
  }
  /*----------------------------------------------------------------------------
   | Read-only data
   *--------------------------------------------------------------------------*/
  .rodata : ALIGN(16)
  {
    SORT(*libboot_first.a:*)(.eh_frame)
    EXCLUDE_FILE(*libboot_last.a:*) *(.eh_frame)
    SORT(*libboot_last.a:*)(.eh_frame)
    *(.gcc_except_table)

    /* These need to be 8-byte aligned */
    . = ALIGN(8);
    __initArrayBegin = .;
    *(.init_array)
    __initArrayEnd = .;

    . = ALIGN(8);
    __finiArrayBegin = .;
    *(.fini_array)
    __finiArrayEnd = .;

    *(.rodata*)
    *(.gnu.linkonce.r*)
  }

  /*----------------------------------------------------------------------------
   | Read-write data (initialized)
   +--------------------------------------------------------------------------*/
  .data : ALIGN(16)
  {
    *(.data*)
    *(.gnu.linkonce.d*)
    *(.got.plt)
  }

  /*----------------------------------------------------------------------------
   | Read-write data (uninitialized) and stack
   *--------------------------------------------------------------------------*/
  .bss : ALIGN(16)
  {
    __bssStart = .;
    *(COMMON)
    *(.bss)
    __bssEnd = .;
  }

  /*----------------------------------------------------------------------------
   | The linker will add other stuff as needed
   +--------------------------------------------------------------------------*/

  /*----------------------------------------------------------------------------
   | We need to know where the kernel ends in case we choose the same region
   | for the heap.
   *--------------------------------------------------------------------------*/
  __kernelEnd = .;
}
