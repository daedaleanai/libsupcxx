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

#include "../bootinfo.hh"

#include <io/string.hh>

// These variables do not exist, but their addresses are known to the linker.
// See the linker script for details.
extern uint32_t __kernelEnd;
extern uint32_t __initArrayBegin;
extern uint32_t __initArrayEnd;
extern uint32_t __finiArrayBegin;
extern uint32_t __finiArrayEnd;

BootInfo bootInfo;

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
  const char *vcBase = io::strstr(cmdline, "vc_mem.mem_base=");
  if (vcBase) {
    // The bootloader inserts two space between the boot data and the
    // user-supplied commandline
    const char *userCmdLine = io::strstr(cmdline, "  ");
    if (userCmdLine) {
      info->cmdline = userCmdLine + 2;
    } else {
      info->cmdline = "";
    }

    // Figure out where our memory ends - this is where the videocore memory
    // starts. At leas I thinks so. Could not find any docs.
    vcBase += io::strlen("vc_mem.mem_base=") + 2; // name + '0x'
    const char *ptr = vcBase;
    uint64_t heapEnd = io::strtoul(vcBase, &ptr, 16);
    if (ptr != vcBase && *ptr == ' ') {
      info->heapEnd = heapEnd;
    }
  }

  // Align the heap to 16 bytes
  info->heapStart = (((info->heapStart-1)>>4)<<4)+16;
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
