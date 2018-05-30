#include <Compiler/FantaC.h>

#include <iostream>

int main(int argc, char **argv) {
  static_cast<void>(argc);
  static_cast<void>(argv);
  fantac::FantaCApp App;
  App.run("test");
}
