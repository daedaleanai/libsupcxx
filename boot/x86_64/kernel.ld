/*------------------------------------------------------------------------------
 | Copyright (C) 2018-2019 Daedalean AG
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
 |----------------------------------------------------------------------------*/

ENTRY(_start)

SECTIONS
{
  /*----------------------------------------------------------------------------
   | 1MB both VMA and LMA - that's where the bootloader will put us.
   *--------------------------------------------------------------------------*/
  . = 1M;

  /*----------------------------------------------------------------------------
   | The text section.
   |
   | We need to put the multiboot header first because the bootloader will
   | look for it in the first 8k of the binary image and, since it's not called
   | by anything, we explicitly KEEP it so that the linker does not optimize
   | it away. GCC assembles both .init and .fini sections out of a bunch object
   | files. Some of the are provided by the compiler runtime and others by us.
   | crt0.o goes first, then crti.o (I renamed it to crt1.o so that I can use
   | sort here), then crtbegin.o (supplied by the compiler runtime, renamed
   | to crt2.o), then crtend.o (supplied by the compiler runtime, renamed to
   | crt3.o), then finally crtn.o (supplied by us, renamed to crt4.o).
   | Together, these create _init and _fini functions compying to the ABI
   | conventions of the given platform that do the initialization and the
   | finalization respectively. Furthermore, we also include the COMDAT
   | sections, to properly handle the inlined functions, vtables, and
   | typeinfos. These are called ".gnu.linkonce." and should be placed in the
   | .text, .rodata, or .data sections respectively.
   *--------------------------------------------------------------------------*/
  .text BLOCK(4K) : ALIGN(4K)
  {
    KEEP(*(.multiboot))
    SORT(*libboot_first.a:*)(.init*)
    SORT(*libboot_last.a:*)(.init*)
    *(.text*)
    *(.gnu.linkonce.t*)
    SORT(*libboot_first.a:*)(.fini*)
    SORT(*libboot_last.a:*)(.fini*)
  }

  /*----------------------------------------------------------------------------
   | Read-only data
   *--------------------------------------------------------------------------*/
  .rodata BLOCK(4K) : ALIGN(4K)
  {
    /*--------------------------------------------------------------------------
     | Constructors and destructors
     |
     | The .ctors and .dtors sections are constructed roughly the same way as
     | as .init and .fini above. First goes whatever is in libboot_first.a
     | in the right order (ie. crt0.o, crt1.o, crt2.o), then whatever
     | constructor or destructor pointers the user code specifies, and finally
     | whatever is in libboot_last.a in the right order (ie. crt3.o and crt4.o).
     | These do not assemble valid function call, but rather a list of pointers
     | that the runtime can easily find in the binary.
     |
     | The gcc runtime's __do_global_ctors_aux (called from the assembled
     | _init) will call the list of constructors in the reverse order to the
     | one that it encounters in the __CTOR_LIST__. Since I want to do testing
     | using qemu, I make sure that the vga.cc's constructors are called
     | before any other constructors except for the heap initializer.
     *------------------------------------------------------------------------*/
    SORT(*libboot_first.a:*)(.ctors*)
    *(SORT(.ctors.*))
    EXCLUDE_FILE(*libboot_last.a:* *libio.a:vga* *libruncxx.a:memory*) *(.ctors)
    *libio.a:vga*(.ctors*)
    *libruncxx.a:memory*(.ctors*)
    SORT(*libboot_last.a:*)(.ctors*)

    SORT(*libboot_first.a:*)(.dtors*)
    *(SORT(.dtors.*))
    EXCLUDE_FILE(*libboot_last.a:*) *(.dtors)
    SORT(*libboot_last.a:*)(.dtors*)

    /*--------------------------------------------------------------------------
     | Libgcc's unwinding code sticks a label at the beginning of the eh_frame
     | information and a sentinel at the end, so that it can easily find this
     | data in the binary. Therefore we need to repeat the same trick we did for
     | the constructors and destructors here as well.
     |
     | The LSDA (Language-Specific Data) will land just after that.
     *------------------------------------------------------------------------*/
    SORT(*libboot_first.a:*)(.eh_frame)
    EXCLUDE_FILE(*libboot_last.a:*) *(.eh_frame)
    SORT(*libboot_last.a:*)(.eh_frame)
    *(.gcc_except_table)

    *(.rodata*)
    *(.gnu.linkonce.r*)
  }

  /*----------------------------------------------------------------------------
   | Read-write data (initialized)
   +--------------------------------------------------------------------------*/
  .data BLOCK(4K) : ALIGN(4K)
  {
    *(.data*)
    *(.gnu.linkonce.d*)

    /*--------------------------------------------------------------------------
     | You might be surprised, as I was, why we need .got.plt in a statically
     | linked binary. The answer is and interesting one. See here:
     |
     | https://reverseengineering.stackexchange.com/questions/2172/why-are-got-
     | and-plt-still-present-in-linux-static-stripped-binaries
     |
     | Since the purpose of PLT stubs is to dispatch cross module calls, it
     | would seem that the targets can only be exported symbols from other
     | modules. However, recent versions of gcc support a new function type
     | called gnu indirect function, which allows a function to have many
     | different implementations, with the most suitable one selected at runtime
     | based on factors such as the CPU type. Currently, many glibc low level
     | functions such as memcpy, strcmp and strlen use this feature. To support
     | this feature, a library exports a chooser function that selects at
     | runtime which of the many implementations is going to be used. These
     | implementation functions may not be exported at all.
     *------------------------------------------------------------------------*/
    *(.got.plt)
  }

  /*----------------------------------------------------------------------------
   | Read-write data (uninitialized) and stack
   *--------------------------------------------------------------------------*/
  .bss BLOCK(4K) : ALIGN(4K)
  {
    *(COMMON)
    *(.bss)
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
