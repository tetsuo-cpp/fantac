#include <Compiler/FantaC.h>

#include <iostream>

int main(int argc, char **argv) {
  // Just use the first argument as the filename.
  if (argc != 2) {
    std::cerr << "Usage: ./fantac [PATH]\n";
    return 1;
  }

  fantac::FantaC Compiler;
  Compiler.run(argv[1]);
  return 0;
}
