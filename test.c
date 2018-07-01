int foo(int x, int y);

int bar(int x) {
  int y = foo(x, x);
  x = y;
  return x;
}

int foo(int x, int y) { return x * y; }
