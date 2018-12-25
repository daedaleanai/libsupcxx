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
     | before any other constructors.
     *------------------------------------------------------------------------*/
    SORT(*libboot_first.a:*)(.ctors*)
    EXCLUDE_FILE(*libboot_last.a:* *libio.a:vga*) *(.ctors)
    *libio.a:vga*(.ctors*)
    SORT(*libboot_last.a:*)(.ctors*)

    SORT(*libboot_first.a:*)(.dtors*)
    EXCLUDE_FILE(*libboot_last.a:*) *(.dtors)
    SORT(*libboot_last.a:*)(.dtors*)

    *(.rodata)
    *(.gnu.linkonce.r*)
  }

  /*----------------------------------------------------------------------------
   | Read-write data (initialized)
   +--------------------------------------------------------------------------*/
  .data BLOCK(4K) : ALIGN(4K)
  {
    *(.data)
    *(.gnu.linkonce.d*)
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
