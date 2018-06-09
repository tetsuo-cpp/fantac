#include "Parser.h"

namespace fantac::parse {

Parser::Parser(const std::vector<Token> &Tokens) : Tokens(Tokens) {}

void Parser::parse() {
  while (auto TopLevelNode = parseTopLevelExpr()) {
    AST.push_back(std::move(TopLevelNode));
  }
}

std::unique_ptr<ast::IASTNode> Parser::parseTopLevelExpr() {
  static_cast<void>(Tokens);
  return std::make_unique<ast::IASTNode>();
}

} // namespace fantac::parse
