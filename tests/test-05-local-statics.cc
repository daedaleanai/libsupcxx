
#include <io/printf.hh>

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
}
