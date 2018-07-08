#pragma once

#include "ParseInterfaces.h"

#include <AST/AST.h>

namespace fantac::parse {

class Parser {
public:
  // TODO: Make ITokenLexer interface less nonsense and then accept it into the
  // Parser ctor.
  Parser(const std::vector<Token> &Tokens);
  virtual ~Parser() = default;

  const std::vector<ast::ASTPtr> &parse();

private:
  // Function to parse a top level expression of a source file.
  ast::ASTPtr parseTopLevelExpr();

  // Helpers to check the current type of top level expression.
  std::vector<Token>::const_iterator isFunctionSig() const;
  bool isFunctionDecl() const;
  bool isFunctionDef() const;

  // Helpers to parse each type of top level expression.
  std::unique_ptr<ast::FunctionDecl> parseFunctionSig();
  ast::ASTPtr parseFunctionDecl();
  ast::ASTPtr parseFunctionDef();

  ast::ASTPtr parseStatement();

  bool isVarDecl() const;

  ast::ASTPtr parseVarDecl();

  const std::vector<Token> &Tokens;
  std::vector<ast::ASTPtr> AST;
  std::vector<Token>::const_iterator TokenIter;
};

} // namespace fantac::parse
