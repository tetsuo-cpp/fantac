#pragma once

#include "ParseInterfaces.h"

#include <AST/AST.h>

#include <memory>

namespace fantac::parse {

class Parser {
public:
  // TODO: Make ITokenLexer interface less nonsense and then accept it into the
  // Parser ctor.
  Parser(const std::vector<Token> &Tokens);
  virtual ~Parser() = default;

  void parse();

private:
  std::unique_ptr<ast::IASTNode> parseTopLevelExpr();

  const std::vector<Token> &Tokens;
  std::vector<std::unique_ptr<ast::IASTNode>> AST;
};

} // namespace fantac::parse
