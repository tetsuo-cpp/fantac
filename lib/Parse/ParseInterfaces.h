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
  TK_Identifier,
  TK_Symbol,
  // Literals.
  TK_NumberLiteral,
  TK_CharLiteral,
  TK_StringLiteral,
  // Keywords.
  TK_If,
  TK_Else,
  TK_Return,
  // Symbols.
  TK_OpenBrace,
  TK_CloseBrace,
  TK_OpenParen,
  TK_CloseParen,
  TK_Comma,
  TK_Semicolon,
  TK_Colon,
  // Operators.
  TK_Add,
  TK_Subtract,
  TK_Multiply,
  TK_Divide,
  // Comparators.
  TK_LessThan,
  TK_GreaterThan,
  TK_Equals,
  // End of file.
  TK_EOF,
  TK_None
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

  TokenKind Kind = TokenKind::TK_None;
  std::string Value;
};

class ILexer {
public:
  virtual ~ILexer() = default;

  virtual bool lex(Token &Tok) = 0;
};

} // namespace fantac::parse
