#include "FantaC.h"

#include <Parse/TokenLexer.h>

#include <fstream>

namespace fantac {

void FantaC::run(const std::string &FileName) {
  std::ifstream File(FileName);

  Lexer.reset(new parse::TokenLexer(std::istreambuf_iterator<char>(File),
                                    std::istreambuf_iterator<char>()));
  try {
    Lexer->lex();
    Parser.reset(new parse::Parser(Lexer->getTokens()));
    Parser->parse();
  } catch (const parse::ParseException &Error) {
    std::cerr << Error.what() << "\n";
  }
}

} // namespace fantac
