#pragma once

#include <Parse/TokenLexer.h>

#include <string>

namespace fantac {

class FantaCApp {
public:
  virtual ~FantaCApp() = default;

  void run(const std::string &FileName);

private:
};

} // namespace fantac
