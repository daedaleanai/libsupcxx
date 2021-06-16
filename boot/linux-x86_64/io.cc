#include "libsupcxx/boot/linux-x86_64/syscall.h"
#include <cstdint>

extern "C" uint64_t _syscall(uint64_t syscall, ...);

extern "C" void _putChar(char c) {
  _syscall(SYS_write, STDOUT_FILENO, &c, 1);
}

extern "C" void abort() {
  _syscall(SYS_exit, 1);
}
