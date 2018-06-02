#pragma once

#include <stdexcept>
#include <string>
#include <vector>

namespace fantac::parse {

// Generic exception for parsing related errors.
class ParseException : public std::runtime_error {
public:
  ParseException(const std::string &Error);
  virtual ~ParseException() = default;
};

ParseException::ParseException(const std::string &Error)
    : std::runtime_error(Error) {}

enum TokenKind { TK_String, TK_Eof };

struct Token {
  template <typename String>
  Token(TokenKind Kind, String &&Id)
      : Kind(Kind), Id(std::forward<String>(Id)) {}
  Token(TokenKind Kind) : Kind(Kind) {}

  TokenKind Kind;
  const std::string Id;
};

class ITokenLexer {
public:
  virtual ~ITokenLexer() = default;

  // TODO: Might be good to provide functions for getting the current token and
  // peeking forward N tokens, etc. Just need to figure out what I need.
  virtual const std::vector<Token> &getTokens() const = 0;
};

} // namespace fantac::parse
