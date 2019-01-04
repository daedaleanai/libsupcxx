
#include <stdint.h>

struct [[gnu::packed]] BootInfo {
  char *cmdline;
  uint64_t heapStart;
  uint64_t heapEnd;
};
