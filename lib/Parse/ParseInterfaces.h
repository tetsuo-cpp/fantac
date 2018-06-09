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
  // Literals.
  TK_Number,
  TK_Char,
  TK_String,
  // Var names, func names and keywords.
  TK_Identifier,
  // Operators, braces, etc.
  TK_Symbol,
  // End of file.
  TK_EOF
};

struct Token {
  template <typename T>
  Token(TokenKind Kind, T &&Value)
      : Kind(Kind), Value(std::forward<T>(Value)) {}
  explicit Token(TokenKind Kind) : Kind(Kind) {}

  TokenKind Kind;
  const std::string Value;
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
