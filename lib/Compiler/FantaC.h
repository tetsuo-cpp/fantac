#pragma once

#include <AST/ASTInterfaces.h>
#include <Parse/ParseInterfaces.h>
#include <Util/UtilInterfaces.h>

#include <memory>
#include <string>

namespace fantac {

class FantaC {
public:
  FantaC();
  virtual ~FantaC() = default;

  void run(const std::string &FileName, const std::string &LoggingLevel);

private:
  std::unique_ptr<parse::ILexer> Lexer;
  std::unique_ptr<parse::IParser> Parser;
  std::vector<std::unique_ptr<ast::IASTVisitor>> ASTVisitors;
  std::unique_ptr<util::ILoggerFactory> LF;
};

} // namespace fantac
