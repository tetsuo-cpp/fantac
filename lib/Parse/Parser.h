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
  ast::ASTPtr parseWhileLoop();
  ast::ASTPtr parseForLoop();
  ast::ASTPtr parseExpr();
  ast::ASTPtr parsePrimaryExpr();
  ast::ASTPtr parseAssignment();
  ast::ASTPtr parseTernary();
  ast::ASTPtr parseLogicalOr();
  ast::ASTPtr parseLogicalAnd();
  ast::ASTPtr parseBitwiseOr();
  ast::ASTPtr parseBitwiseXor();
  ast::ASTPtr parseBitwiseAnd();
  ast::ASTPtr parseEquality();
  ast::ASTPtr parseRelational();
  ast::ASTPtr parseShift();
  ast::ASTPtr parseAddition();
  ast::ASTPtr parseMultiplication();
  ast::ASTPtr parseUnary();
  ast::ASTPtr parsePostfix();

  ILexer &Lexer;
  Token CurrentToken;
  std::unique_ptr<spdlog::logger> Logger;
};

} // namespace fantac::parse
