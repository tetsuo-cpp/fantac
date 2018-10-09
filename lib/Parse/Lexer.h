#pragma once

#include "ParseInterfaces.h"

namespace fantac::parse {

class Lexer : public ILexer {
public:
  Lexer(const char *Begin, const char *End);
  virtual ~Lexer() = default;

  virtual bool lex(Token &Token) override;

private:
  bool lexToken(Token &Token);
  void lexIdentifier(Token &Token);
  void lexNumber(Token &Token);
  void lexChar(Token &Token);
  void lexString(Token &Token);
  bool readNextChar();

  char CurrentChar;
  const char *Current, *End;
};

} // namespace fantac::parse
