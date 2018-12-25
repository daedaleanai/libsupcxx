
#include <io/vga.hh>

class TestGlobalCtorDtor {
public:
  TestGlobalCtorDtor() {
    io::vgaTerminalWriteString("Ctor called!\n");
  }
  ~TestGlobalCtorDtor() {
    io::vgaTerminalWriteString("Dtor called!\n");
  }
};

TestGlobalCtorDtor tmp;

void main() {
  // The call below depends on the VGA terminal being initialized by a global
  // object with a constructor and constexpr functions. See io/vga.cc.
  io::vgaTerminalWriteString("Hello, Kernel World!\nAnd Hello Again!\n");
}
