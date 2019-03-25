#pragma once

#include "ParseInterfaces.h"

namespace fantac::parse {

class Lexer : public ILexer {
public:
  Lexer(const char *, const char *);
  virtual ~Lexer() = default;

  // ILexer impl.
  bool lex(Token &) override;

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
