
#include <io/printf.hh>

#include <exception>

class TestException: public std::exception {
public:
  TestException() {
    io::printf("Construct test exception\n");
  }
  ~TestException() {
    io::printf("Destruct test exception\n");
  }

  virtual const char *what() const noexcept override {
    return "Test exception";
  }
};

void six() {
  throw TestException();
}

void five() {
  six();
  io::printf("If you see this, something has gone wrong (five)\n");
}

int four(int arg) {
  try {
    five();
  } catch (TestException &e) {
    io::printf("Exception caught in four: %s\n", e.what());
    io::printf("Rethrowing...\n");
    throw;
  }
  return 22 + arg;
}

int three() {
  return 10;
}

int two(int arg) {
  return arg + 12;
}

int one() {
  int ret = two(10);
  try {
    ret += three();
    ret += four(20);
    io::printf("If you see this, something has gone wrong (one)\n");
  } catch (TestException &e) {
    io::printf("Exception caught in one: %s\n", e.what());
    ret += 10;
  }
  return ret;
}

void main(const char *cmdline) {
  int test = one();
  io::printf("Test result: %d (expected 42)\n", test);
}
