#include <stdio.h>

int printi(int x);

int foo(int x, int y);

int bar(int x) {
  int y = foo(x, x);
  printi(x);
  printi(y);
  if (x > y) {
    printi(x);
  } else {
    printi(99);
  }

  return x;
}

int foo(int x, int y) {
  int z;
  return 100;
}
