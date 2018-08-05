#pragma once

#include "ParseInterfaces.h"

namespace fantac::parse {

class Lexer : public ILexer {
public:
  Lexer(const char *Begin, const char *End);
  virtual ~Lexer() = default;

  virtual bool lex(Token &) override;

private:
  bool lexToken(Token &);
  void lexIdentifier(Token &);
  void lexNumber(Token &);
  void lexChar(Token &);
  void lexString(Token &);
  bool readNextChar();

  char CurrentChar;
  const char *Current, *End;
};

} // namespace fantac::parse
