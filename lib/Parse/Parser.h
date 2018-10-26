#pragma once

#include "ParseInterfaces.h"

#include <AST/AST.h>
#include <Util/LoggerFactory.h>

namespace fantac::parse {

class Parser {
public:
  Parser(ILexer &Lexer, util::LoggerFactory &LF);
  virtual ~Parser() = default;

  ast::ASTPtr parseTopLevelExpr();

private:
  bool consumeToken(TokenKind Kind);
  void expectToken(TokenKind Kind);
  ast::ASTPtr parseFunction(ast::CTypeKind Return, std::string &&Name);
  ast::ASTPtr parseStatement();

  ILexer &Lexer;
  Token CurrentToken;
  spdlog::logger &Logger;
};

} // namespace fantac::parse
