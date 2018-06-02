#pragma once

#include <string>

namespace fantac::parse {

class ParseException : public std::runtime_error {
public:
  ParseException(const std::string &Error);
  virtual ~ParseException() = default;
};

ParseException::ParseException(const std::string &Error)
    : std::runtime_error(Error) {}

} // namespace fantac::parse
