
#include "string.hh"

namespace io {
size_t strlen(const char *str)
{
  size_t len;
  for (len = 0; *str; ++str, ++len) {};
  return len;
}

} // namespace io
