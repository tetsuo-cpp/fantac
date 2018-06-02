#pragma once

#include <stdexcept>
#include <string>
#include <vector>

namespace fantac::parse {

// Generic exception for parsing related errors.
class ParseException : public std::runtime_error {
public:
  template <typename T> ParseException(T &&Error);
  virtual ~ParseException() = default;
};

template <typename T>
ParseException::ParseException(T &&Error)
    : std::runtime_error(std::forward<T>(Error)) {}

enum TokenKind {
  TK_Keyword,
  TK_Number,
  TK_Char,
  TK_String,
  TK_Identifier,
  TK_EOF
};

struct Token {
  template <typename T>
  Token(TokenKind Kind, T &&Symbol)
      : Kind(Kind), Symbol(std::forward<T>(Symbol)) {}
  explicit Token(TokenKind Kind) : Kind(Kind) {}

  TokenKind Kind;
  const std::string Symbol;
};

class ITokenLexer {
public:
  virtual ~ITokenLexer() = default;

  // TODO: Might be good to provide functions for getting the current token and
  // peeking forward N tokens, etc. Just need to figure out what I need.
  virtual void lex() = 0;
  virtual const std::vector<Token> &getTokens() const = 0;
};

} // namespace fantac::parse
