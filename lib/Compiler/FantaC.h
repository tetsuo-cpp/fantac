#pragma once

#include <Parse/Parser.h>
#include <Parse/ParseInterfaces.h>

#include <memory>
#include <string>

namespace fantac {

class FantaC {
public:
  virtual ~FantaC() = default;

  void run(const std::string &FileName);

private:
  std::unique_ptr<parse::ITokenLexer> Lexer;
  std::unique_ptr<parse::Parser> Parser;
};

} // namespace fantac
