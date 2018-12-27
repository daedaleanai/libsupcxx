
extern "C" void abort() {
  asm volatile ("hlt");
}
