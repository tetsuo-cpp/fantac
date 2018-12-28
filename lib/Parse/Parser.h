#pragma once

#include "ParseInterfaces.h"
#include "Token.h"

#include <AST/ASTInterfaces.h>
#include <Util/UtilInterfaces.h>

namespace fantac::ast {

struct CType;
enum class CTypeKind;

} // namespace fantac::ast

namespace fantac::parse {

class Parser : public IParser {
public:
  Parser(ILexer &Lexer, util::ILoggerFactory &LF);
  virtual ~Parser() = default;

  ast::ASTPtr parseTopLevelExpr() override;

private:
  bool consumeToken(TokenKind Kind);
  void expectToken(TokenKind Kind);
  ast::ASTPtr parseFunction(ast::CType Return, std::string &&Name);
  ast::ASTPtr parseStatement();
  ast::ASTPtr parseVariableDecl(ast::CType Type);
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
  ast::ASTPtr parseFunctionCall(std::string &&FunctionName);
  ast::CType parseType();

  ILexer &Lexer;
  Token CurrentToken;
  std::unique_ptr<spdlog::logger> Logger;
};

} // namespace fantac::parse
