#include <stdio.h>

extern "C" int putchari(int X) {
  fputc((char)X, stdout);
  return 0;
}

extern "C" int printi(int X) {
  fprintf(stdout, "%d\n", X);
  return 0;
}

extern "C" float printfl(float X) {
  fprintf(stdout, "%f\n", X);
  return 0.0;
}

extern "C" {
float bar(float);
}

int main() {
  printf("Running Bar.\n");
  bar(2.0);
  bar(-1.3);
  printf("Exiting Bar.\n");
}
