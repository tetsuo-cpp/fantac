#pragma once

#include "ParseInterfaces.h"

#include <Util/UtilInterfaces.h>

namespace fantac::parse {

class Lexer : public ILexer {
public:
  Lexer(const char *Begin, const char *End, util::ILoggerFactory &LF);
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
  std::unique_ptr<spdlog::logger> Logger;
};

} // namespace fantac::parse
