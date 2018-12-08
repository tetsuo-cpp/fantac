#pragma once

#include <memory>
#include <stdexcept>

namespace fantac::ast {

struct IAST;

} // namespace fantac::ast

namespace fantac::parse {

// Generic exception for parsing related errors.
class ParseException : public std::runtime_error {
public:
  template <typename T>
  explicit ParseException(T &&Error)
      : std::runtime_error(std::forward<T>(Error)) {}
  virtual ~ParseException() = default;
};

struct Token;

class ILexer {
public:
  virtual ~ILexer() = default;

  virtual bool lex(Token &Tok) = 0;
};

class IParser {
public:
  virtual ~IParser() = default;

  virtual std::unique_ptr<ast::IAST> parseTopLevelExpr() = 0;
};

} // namespace fantac::parse
