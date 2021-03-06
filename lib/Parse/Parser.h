#pragma once

#include "ParseInterfaces.h"
#include "Token.h"

#include <AST/ASTInterfaces.h>

namespace fantac::ast {

struct CType;
enum class CTypeKind;

} // namespace fantac::ast

namespace fantac::parse {

class Parser : public IParser {
public:
  explicit Parser(ILexer &);
  virtual ~Parser() = default;

  // IParser impl.
  ast::ASTPtr parseTopLevelExpr() override;

private:
  bool consumeToken(TokenKind);
  void expectToken(TokenKind);
  ast::ASTPtr parseFunction(ast::CType, std::string &&);
  ast::ASTPtr parseStatement();
  ast::ASTPtr parseVariableDecl(ast::CType);
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
  ast::ASTPtr parseFunctionCall(std::string &&);
  ast::CType parseType();

  ILexer &Lexer;
  Token CurrentToken;
};

} // namespace fantac::parse
