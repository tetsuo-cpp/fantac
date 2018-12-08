#pragma once

#include <AST/ASTInterfaces.h>
#include <Parse/ParseInterfaces.h>
#include <Util/LoggerFactory.h>

#include <memory>
#include <string>

namespace fantac {

class FantaC {
public:
  FantaC();
  virtual ~FantaC() = default;

  void run(const std::string &FileName);

private:
  std::unique_ptr<parse::ILexer> Lexer;
  std::unique_ptr<parse::IParser> Parser;
  std::vector<std::unique_ptr<ast::IASTVisitor>> ASTVisitors;
  const util::LoggerConfig LFConfig;
  util::LoggerFactory LF;
};

} // namespace fantac
