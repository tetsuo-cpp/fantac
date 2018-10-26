#pragma once

#include "ParseInterfaces.h"

#include <Util/LoggerFactory.h>

namespace fantac::parse {

class Lexer : public ILexer {
public:
  Lexer(const char *Begin, const char *End, util::LoggerFactory &LF);
  virtual ~Lexer() = default;

  virtual bool lex(Token &Tok) override;

private:
  bool lexToken(Token &Tok);
  void lexIdentifier(Token &Tok);
  void lexNumber(Token &Tok);
  void lexChar(Token &Tok);
  void lexString(Token &Tok);
  bool readNextChar();

  char CurrentChar;
  const char *Current, *End;
  spdlog::logger &Logger;
};

} // namespace fantac::parse
