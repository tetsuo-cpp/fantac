#include "FantaC.h"

#include <fstream>

namespace fantac {

void FantaCApp::run(const std::string &FileName) {
  std::ifstream File(FileName);
  parse::TokenLexer Lexer;
  Lexer.read(std::istreambuf_iterator<char>(File),
             std::istreambuf_iterator<char>());
}

} // namespace fantac
