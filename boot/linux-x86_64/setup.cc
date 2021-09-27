//------------------------------------------------------------------------------
// Copyright (C) 2021 Daedalean AG
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
#include "libsupcxx/boot/linux-x86_64/syscall.h"

#include <cstring>
#include <cstdint>

// The length of the heap to allocate can be specified by an environment
// variable
static const char *ENV_HEAPLEN = "LIBSUPCXX_HEAPLEN";
static const size_t HEAPLEN_DEFAULT = 1 << 26; // 64 MiB

static const char *MMAP_ERR_MSG = "libsupcxx setup error\n";

extern "C" uint64_t _syscall(uint64_t syscall, ...);

extern BootInfo bootInfo;

// Similar to standard getenv, but takes envp as argument
extern "C" const char *_getenv(const char **envp, const char *name) {
  size_t len = strlen(name);
  for (const char **env = envp; *env; ++env) {
    if (strncmp(*env, name, len) == 0) {
      return *env + len + 1;
    }
  }
  return nullptr;
}

// The length of the heap to be allocated (from env or default)
extern "C" size_t _heapLen(const char **envp) {
  const char *env = _getenv(envp, ENV_HEAPLEN);
  if (env) {
    return strtoul(env, nullptr, 0);
  }
  return HEAPLEN_DEFAULT;
}

// Add the cmdline (argv's elements, space-separated) to the start of the heap
extern "C" void _addCmdLine(size_t argc, const char **argv, char *heap) {
  char *end = heap;
  for (size_t i = 0; i < argc; ++i) {
    size_t len = strlen(argv[i]);
    strncpy(end, argv[i], len);
    if (i == argc - 1) {
      end[len] = '\0';
    } else {
      end[len] = ' ';
    }
    end = &end[len + 1];
  }
  return;
}

// Called by start, allocates heap and populates bootInfo
extern "C" void _systemSetup(size_t argc, const char **argv,
                             const char **envp) {
  size_t heapLen = _heapLen(envp);
  int64_t mmapReturn =
      _syscall(SYS_mmap,               // mmap(
               nullptr,                // addr, (let the kernel choose)
               heapLen,                // length, (from environment / default)
               PROT_READ | PROT_WRITE, // prot, (rw)
               MAP_PRIVATE | MAP_ANONYMOUS, // flags, (not file-mapped)
               -1,                          // fd, (not file-mapped)
               0                            // offset (none)
      );

  // Check for failure. Unlike mmap, SYS_mmap can return a range of errno's
  if (mmapReturn >= -4095 && mmapReturn <= -1) {
    _syscall(SYS_write, STDOUT_FILENO, MMAP_ERR_MSG, strlen(MMAP_ERR_MSG));
    _syscall(SYS_exit, 1);
  }

  char *allocated = (char *)mmapReturn;

  // Copy the command line at the start of the allocated memory
  _addCmdLine(argc, argv, (char *)allocated);

  // There is only one memory region, i.e. what's been allocated
  bootInfo.numMemoryRegions = 1;
  bootInfo.cmdline = (const char *)allocated;
  bootInfo.memoryMap[0].start = (uint64_t)allocated;
  bootInfo.memoryMap[0].end = (uint64_t)(allocated + heapLen);
  bootInfo.memoryMap[0].offset = 0;
  bootInfo.memoryMap[0].type = MemoryType::RAM;
}
