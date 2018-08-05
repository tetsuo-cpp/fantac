#pragma once

#include <stdexcept>
#include <string>
#include <vector>

namespace fantac::parse {

// Generic exception for parsing related errors.
class ParseException : public std::runtime_error {
public:
  template <typename TString>
  ParseException(TString &&Error)
      : std::runtime_error(std::forward<TString>(Error)) {}
  virtual ~ParseException() = default;
};

enum class TokenKind {
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
  template <typename TString> void assign(TokenKind Kind, TString &&Value) {
    this->Kind = Kind;
    this->Value = std::forward<TString>(Value);
  }

  void assign(TokenKind Kind) {
    this->Kind = Kind;
    Value.clear();
  }

  TokenKind Kind = TokenKind::TK_EOF;
  std::string Value;
};

class ILexer {
public:
  virtual ~ILexer() = default;

  virtual bool lex(Token &Tok) = 0;
};

} // namespace fantac::parse
