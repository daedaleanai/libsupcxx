
#include <stdint.h>

struct BootInfo {
  char *cmdline;
  uint64_t heapStart;
  uint64_t heapEnd;
}  __attribute__((packed));
