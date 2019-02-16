#include <stdio.h>

void printf(char *msg);
int printi(int x);
float printfl(float X);

float foo(float x, float y);

float bar(float x) {
  printf("Testing printf.");
  float y = foo(x, x);
  printfl(x);
  printfl(y);
  if (x > y) {
    printfl(x);
  } else {
    printfl(99.3);
  }

  printf("Executing loop.");
  int i = 0;
  while (i < 10) {
    printi(i);
    i = i + 1;
  }

  return x;
}

float foo(float x, float y) { return 100.0; }
