int foo(int x, int y);

int bar(int x) {
  int y = foo(x, x);
  if (x >= y) {
    x = y;
  }

  return x;
}

int foo(int x, int y) {
  x = 1;
  int z;
  return x * y;
}
