
#include "libsupcxx/io/printf.hh"

int recursiveInit(int i) {
  io::printf("recursiveInit(%d)!\n", i);
  static int s = recursiveInit(i+1);    // recursive call – undefined
  return i+s;
}

int computeTheMeaningOfLife() {
  io::printf("Computing the meaning of life!\n");
  return 42;
}

int getTheMeaningOfLife() {
  io::printf("Getting the meaning of life!\n");
  static int theMeaningOfLife = computeTheMeaningOfLife();
  return theMeaningOfLife;
}

void main(const char *cmdline) {
  int theMeaningOfLife = getTheMeaningOfLife();
  io::printf("Answer to the Ultimate Question of Life, the Universe, and "
             "Everything: %d\n", theMeaningOfLife);

  theMeaningOfLife = getTheMeaningOfLife();
  io::printf("Answer to the Ultimate Question of Life, the Universe, and "
             "Everything: %d\n", theMeaningOfLife);

  recursiveInit(0);
  io::printf("You should not be seeing this\n");
}
