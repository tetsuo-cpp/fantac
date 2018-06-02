#include "FantaC.h"

#include <Parse/TokenLexer.h>

#include <fstream>

namespace fantac {

void FantaC::run(const std::string &FileName) {
  std::ifstream File(FileName);

  Lexer.reset(new parse::TokenLexer(std::istreambuf_iterator<char>(File),
                                    std::istreambuf_iterator<char>()));
  Lexer->lex();
}

} // namespace fantac
