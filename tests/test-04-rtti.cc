
#include "libsupcxx/io/printf.hh"

#include <typeinfo>

class IInterface {
public:
  virtual ~IInterface() {}
  virtual void f() {};
};

class Impl1: public IInterface {
};

class Impl2: public IInterface {
};

template <typename T>
class Number {
  T value;
public:
  Number(T n) {
    value = n;
  }
  T get_value() {
    return value;
  };
};

void main(const char *cmdline) {
  // builtins
  char c, *p;
  int i;
  long l;
  float f;
  double d;

  io::printf("--- builtins ---\n");
  io::printf("char's name: %s\n", typeid(c).name());
  io::printf("char *'s name: %s\n", typeid(p).name());
  io::printf("int's name: %s\n", typeid(i).name());
  io::printf("long's name: %s\n", typeid(l).name());
  io::printf("float's name: %s\n", typeid(f).name());
  io::printf("double's name: %s\n", typeid(d).name());

  // classes
  IInterface ib, *ptr;
  Impl1 i1;
  Impl2 i2;

  io::printf("--- polymorphic types ---\n");
  ptr = &ib;
  io::printf("*ptr(ib)'s name: %s\n", typeid(*ptr).name());
  ptr = &i1;
  io::printf("*ptr(i1)'s name: %s\n", typeid(*ptr).name());
  ptr = &i2;
  io::printf("*ptr(i2)'s name: %s\n", typeid(*ptr).name());

  Impl1 *ip1 = dynamic_cast<Impl1*>(ptr);
  if (ip1) {
    io::printf("Impl2 ptr did cast to Impl1 ptr\n");
  } else {
    io::printf("Impl2 ptr did not cast to Impl1 ptr\n");
  }

  Impl2 *ip2 = dynamic_cast<Impl2*>(ptr);
  if (ip2) {
    io::printf("Impl2 ptr did cast to Impl2 ptr\n");
  } else {
    io::printf("Impl2 ptr did not cast to Impl2 ptr\n");
  }

  if (typeid(IInterface) == typeid(Impl1)) {
    io::printf("Impl1 is the same as IInterface\n");
  } else {
    io::printf("Impl1 is not the same as IInterface\n");
  }
  if (typeid(Impl2) == typeid(Impl1)) {
    io::printf("Impl1 is the same as Impl2\n");
  } else {
    io::printf("Impl1 is not the same as Impl2\n");
  }

  // templates
  Number<int> ni(10);
  Number<double> nd(3.14);

  io::printf("--- templates ---\n");
  io::printf("ni's name: %s\n", typeid(ni).name());
  io::printf("nd's name: %s\n", typeid(nd).name());
  if (typeid(Number<double>) == typeid(Number<int>)) {
    io::printf("Number<double> is the same as Number<int>\n");
  } else {
    io::printf("Number<double> is not the same as Number<int>\n");
  }
}
