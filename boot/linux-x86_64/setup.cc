#include "libsupcxx/boot/bootinfo.hh"
#include "libsupcxx/boot/linux-x86_64/syscall.h"
#include "libsupcxx/io/string.hh"

#include <cstdint>

// The length of the heap to allocate can be specified by an environment
// variable
static const char *ENV_HEAPLEN = "HEAPLEN";
static const size_t HEAPLEN_DEFAULT = 1 << 26; // 64 MiB

static const char *MMAP_ERR_MSG = "libsupcxx setup error\n";

extern "C" uint64_t _syscall(uint64_t syscall, ...);

BootInfo bootInfo;

// Similar to standard getenv, but takes envp as argument
extern "C" const char *_getenv(const char **envp, const char *name) {
  size_t len = io::strlen(name);
  for (const char **env = envp; *env; ++env) {
    if (io::strncmp(*env, name, len) == 0) {
      return *env + len + 1;
    }
  }
  return nullptr;
}

// The length of the heap to be allocated (from env or default)
extern "C" size_t _heapLen(const char **envp) {
  const char *env = _getenv(envp, ENV_HEAPLEN);
  if (env) {
    return io::strtoul(env, nullptr, 0);
  }
  return HEAPLEN_DEFAULT;
}

// Add the cmdline (argv's elements, space-separated) to the start of the heap
// Return the new start of the heap (after the cmdline)
extern "C" char *_addCmdLine(size_t argc, const char **argv, char *heap) {
  char *end = heap;
  for (size_t i = 0; i < argc; ++i) {
    size_t len = io::strlen(argv[i]);
    io::strncpy(end, argv[i], len);
    if (i == argc - 1) {
      end[len] = '\0';
    } else {
      end[len] = ' ';
    }
    end = &end[len + 1];
  }
  return end;
}

// Called by start, allocates heap and populates bootInfo
extern "C" void _systemSetup(size_t argc, const char **argv,
                             const char **envp) {
  // MMap the right amout of heap and populate bootInfo
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
    _syscall(SYS_write, STDOUT_FILENO, MMAP_ERR_MSG, io::strlen(MMAP_ERR_MSG));
    _syscall(SYS_exit, 1);
  }

  char *allocated = (char *)mmapReturn;

  // We use the start of the memory we got from mmap to save the cmdline
  // So the real heap starts after end of the cmdline
  bootInfo.heapStart = (uint64_t)_addCmdLine(argc, argv, (char *)allocated);
  bootInfo.cmdline = (const char *)allocated;
  bootInfo.heapEnd = (uint64_t)((uint8_t *)bootInfo.heapStart + heapLen);
}
