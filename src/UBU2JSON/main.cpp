#include "ubu.h"

#include <iostream>

int main(int argc, char *argv[]) {
  ubu::init();

  if (argc != 3 && argc != 4) {
    ubu::usage();
    return 1;
  }

  bool success;
  if (argc == 4) {
    success = ubu::load(argv[1], argv[2], argv[3]);
  }
  else {
    success = ubu::load(argv[1], argv[2]);
  }

  if (!success) {
    ubu::usage();
    return 2;
  }

  return 0;
}
