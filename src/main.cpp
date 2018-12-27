#include <Compiler/FantaC.h>

#include <iostream>

int main(int argc, char **argv) {
  // Just use the first argument as the filename.
  if (argc < 3) {
    std::cerr << "Usage: ./fantac [PATH] [LOGGING_LEVEL]\n";
    return 1;
  }

  fantac::FantaC Compiler;
  Compiler.run(argv[1], argv[2]);
  return 0;
}
