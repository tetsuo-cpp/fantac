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
  ast::ASTPtr parseTopLevelExpr();

  const std::vector<Token> &Tokens;
  std::vector<ast::ASTPtr> AST;
  size_t TokenIndex;
};

} // namespace fantac::parse
