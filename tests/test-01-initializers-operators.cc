
#include "libsupcxx/io/printf.hh"

#include <initializer_list>

class Matrix {
private:
  double matrix_[16];
public:
  Matrix() {
    for(unsigned i = 0; i < 16; ++i) {
      matrix_[i] = 0;
    }
  }

  Matrix(std::initializer_list<double> init) {
    int i = 0;
    for(auto &el : init) {
      matrix_[i++] = el;
    }
  }

  double operator() (unsigned int i, unsigned int j) const {
    return matrix_[i*4+j];
  }

  double &operator() (unsigned int i, unsigned int j) {
    return matrix_[i*4+j];
  }

  Matrix &operator=(const Matrix &rhs) {
    for(unsigned i = 0; i < 16; ++i) {
      matrix_[i] = rhs.matrix_[i];
    }
    return *this;
  }

  Matrix(const Matrix &rhs) {
    for(unsigned i = 0; i < 15; ++i) {
      matrix_[i] = rhs.matrix_[i];
    }
  }
};

Matrix operator * (const Matrix &lhs, const Matrix &rhs) {
  Matrix m;
  for(unsigned i = 0; i < 4; ++i) {
    for(unsigned j = 0; j < 4; ++j) {
      m(i, j) = 0;
      for(unsigned k = 0; k < 4; ++k) {
        m(i, j) += lhs(i, k) * rhs(k, j);
      }
    }
  }
  return m;
}

void printMatrix(const Matrix &a) {
  for (size_t i = 0; i < 4; ++i) {
    for(size_t j = 0; j < 4; ++j) {
      io::printf("%f ", a(i, j));
    }
    io::printf("\n");
  }
}

void main(const char *cmdline) {
  Matrix a = {
     1,  2,  3,  4,
     5,  6,  7,  8,
     9, 10, 11, 12,
    13, 14, 15, 16
  };
  io::printf("a =\n");
  printMatrix(a);

  Matrix b = {
     2,  3,  4,  5,
     6,  7,  8,  9,
    10, 11, 12, 13,
    14, 15, 16, 17
  };
  io::printf("b =\n");
  printMatrix(b);

  Matrix c = a * b;
  io::printf("a * b =\n");
  printMatrix(c);
}
