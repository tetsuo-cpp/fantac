#pragma once

#include "ParseInterfaces.h"

#include <AST/AST.h>

namespace fantac::parse {

class Parser {
public:
  Parser(const std::vector<Token> &Tokens);
  virtual ~Parser() = default;

  ast::ASTPtr parseTopLevelExpr();

private:
  std::vector<Token>::const_iterator isFunctionSig() const;
  bool isFunctionDecl() const;
  bool isFunctionDef() const;
  bool isVarDecl() const;
  std::unique_ptr<ast::FunctionDecl> parseFunctionSig();
  ast::ASTPtr parseFunctionDecl();
  ast::ASTPtr parseFunctionDef();
  ast::ASTPtr parseStatement();
  ast::ASTPtr parseVarDecl();

  const std::vector<Token> &Tokens;
  std::vector<ast::ASTPtr> AST;
  std::vector<Token>::const_iterator TokenIter;
};

} // namespace fantac::parse
