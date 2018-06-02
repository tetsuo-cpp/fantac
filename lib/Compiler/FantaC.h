#pragma once

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
};

} // namespace fantac
