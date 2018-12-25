
#include <cstddef>

namespace io {
extern "C" void *malloc(size_t size);
extern "C" void free(void *ptr);
} // namespace io
