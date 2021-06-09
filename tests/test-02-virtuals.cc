
#include "libsupcxx/io/printf.hh"

class IInterface {
public:
  virtual void doStuff() = 0;
};

class Impl1: public IInterface {
public:
  virtual void doStuff() override {
    io::printf("Do stuff #1\n");
  }
};

class Impl2: public IInterface {
public:
  virtual void doStuff() override {
    io::printf("Do stuff #2\n");
  }
};

void doStuff(IInterface *a) {
  a->doStuff();
}

void main(const char *cmdline) {
  Impl1 a;
  Impl2 b;

  io::printf("Call doStuff(&a)\n");
  doStuff(&a);
  io::printf("Call doStuff(&b)\n");
  doStuff(&b);
}
