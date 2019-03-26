#include <Compiler/FantaC.h>

#include <fmt/format.h>

int main(int argc, char **argv) {
  // Just use the first argument as the filename.
  if (argc < 2) {
    fmt::print("Usage: ./fantac [PATH]\n");
    return 1;
  }

  fantac::run(argv[1]);
  return 0;
}
