#include <iostream>
#include <stdio.h>

extern "C" int putchari(int X) {
  fputc((char)X, stderr);
  return 0;
}

extern "C" int printi(int X) {
  fprintf(stderr, "%d\n", X);
  return 0;
}

extern "C" {
int bar(int);
}

int main() {
  std::cout << "Running Bar." << std::endl;
  bar(2);
  bar(-1);
  std::cout << "Exiting Bar." << std::endl;
}
