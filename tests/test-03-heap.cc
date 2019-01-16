
#include <io/printf.hh>
#include <io/memory.hh>

#include <new>

class IInterface {
public:
  virtual ~IInterface() {
    io::printf("IInterface destructor called\n");
  }
  virtual void doStuff() = 0;
};

class Impl1: public IInterface {
private:
  int a_;
public:
  Impl1(int a = 12) {
    a = a_;
  }
  virtual ~Impl1() {
    io::printf("Impl1 destructor called\n");
  }
  virtual void doStuff() override {
    io::printf("Do stuff #1\n");
  }
};

class Impl2: public IInterface {
public:
  virtual ~Impl2() {
    io::printf("Impl2 destructor called\n");
  }
  virtual void doStuff() override {
    io::printf("Do stuff #2\n");
  }
};

void main(const char *cmdline) {
  char *tmpc = (char*)io::malloc(12);
  io::free(tmpc);

  double *tmpd = new double[12];
  delete [] tmpd;

  io::printf("--- Object construct->destruct-parent ---\n");
  IInterface *a = new Impl1(42);
  IInterface *b = new Impl2;

  delete a;
  delete b;

  io::printf("--- Object array construct->destruct-parent ---\n");
  IInterface *interfaces = new Impl2[2];
  delete [] interfaces;

  io::printf("--- Object construct->destruct-parent (nothrow) ---\n");
  a = new (std::nothrow) Impl1(42);
  b = new (std::nothrow) Impl2;

  delete a;
  delete b;

  io::printf("--- Object array construct->destruct-parent (nothrow) ---\n");
  interfaces = new (std::nothrow) Impl2[2];
  delete [] interfaces;

  io::printf("--- Large array of basic types->destruct ---\n");
  uint64_t *large = new (std::nothrow) uint64_t[8000000];
  io::printf("Address: %p\n", large);

  for(size_t i = 0; i < 8000000; ++i) {
    large[i] = 0;
  }
  io::printf("Cleared up the memory\n");
  delete [] large;
}
