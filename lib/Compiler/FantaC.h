#pragma once

#include <string>

namespace fantac {

class FantaCApp {
public:
  virtual ~FantaCApp() = default;

  void run(const std::string &FileName);

private:
};

} // namespace fantac
