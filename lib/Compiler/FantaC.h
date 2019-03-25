#pragma once

#include <AST/ASTInterfaces.h>
#include <Parse/ParseInterfaces.h>

#include <memory>
#include <string>
#include <vector>

namespace fantac {

class FantaC {
public:
  FantaC();
  virtual ~FantaC() = default;

  void run(const std::string &);

private:
  std::unique_ptr<parse::ILexer> Lexer;
  std::unique_ptr<parse::IParser> Parser;
  std::vector<std::unique_ptr<ast::IASTVisitor>> ASTVisitors;
};

} // namespace fantac
