#pragma once

#include <AST/AST.h>
#include <Parse/ParseInterfaces.h>
#include <Parse/Parser.h>
#include <Util/LoggerFactory.h>

#include <memory>
#include <string>

namespace fantac {

class FantaC {
public:
  virtual ~FantaC() = default;

  void run(const std::string &FileName);

private:
  std::unique_ptr<parse::ILexer> Lexer;
  std::unique_ptr<parse::Parser> Parser;
  std::vector<std::unique_ptr<ast::IASTVisitor>> ASTVisitors;
  util::LoggerFactory LF;
};

} // namespace fantac
