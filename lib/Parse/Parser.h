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
  ast::ASTPtr parseVariableDecl(ast::CTypeKind Type);
  ast::ASTPtr parseIfCond();
  ast::ASTPtr parseExpr();

  ILexer &Lexer;
  Token CurrentToken;
  std::unique_ptr<spdlog::logger> Logger;
};

} // namespace fantac::parse
