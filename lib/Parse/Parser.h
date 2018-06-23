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

  void parse();

private:
  // Function to parse a top level expression of a source file.
  ast::ASTPtr parseTopLevelExpr();

  // Helpers to check the current type of top level expression.
  bool isFunctionDef() const;
  bool isFunctionDec() const;

  // Helpers to parse each type of top level expression.
  ast::ASTPtr parseFunctionDef();
  ast::ASTPtr parseFunctionDec();

  const std::vector<Token> &Tokens;
  std::vector<ast::ASTPtr> AST;
  std::vector<Token>::const_iterator TokenIter;
};

} // namespace fantac::parse
