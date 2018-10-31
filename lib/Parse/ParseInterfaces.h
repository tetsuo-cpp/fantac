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
  TK_For,
  TK_While,
  TK_Return,
  // Symbols.
  TK_OpenBrace,
  TK_CloseBrace,
  TK_OpenParen,
  TK_CloseParen,
  TK_Comma,
  TK_Semicolon,
  TK_Colon,
  TK_Question,
  // Operators.
  TK_Add,
  TK_AddEq,
  TK_Subtract,
  TK_SubtractEq,
  TK_Multiply,
  TK_MultiplyEq,
  TK_Divide,
  TK_DivideEq,
  TK_Modulus,
  TK_ModulusEq,
  TK_Assign,
  // Comparators.
  TK_LessThan,
  TK_LessThanEq,
  TK_GreaterThan,
  TK_GreaterThanEq,
  TK_Equals,
  TK_NotEquals,
  // Bit shifting.
  TK_ShiftLeft,
  TK_ShiftLeftEq,
  TK_ShiftRight,
  TK_ShiftRightEq,
  // Logic.
  TK_And,
  TK_AndEq,
  TK_Or,
  TK_OrEq,
  TK_Xor,
  TK_XorEq,
  TK_LogicalAnd,
  TK_LogicalOr,
  // Postfix.
  TK_Period,
  TK_Arrow,
  TK_Increment,
  TK_Decrement,
  // End of file.
  TK_EOF,
  TK_None
};

inline std::string tokenKindToString(TokenKind Kind) {
  switch (Kind) {
  case TokenKind::TK_Identifier:
    return "Identifier";
  case TokenKind::TK_Symbol:
    return "Symbol";
  case TokenKind::TK_NumberLiteral:
    return "NumberLiteral";
  case TokenKind::TK_CharLiteral:
    return "CharLiteral";
  case TokenKind::TK_StringLiteral:
    return "StringLiteral";
  case TokenKind::TK_If:
    return "If";
  case TokenKind::TK_Else:
    return "Else";
  case TokenKind::TK_For:
    return "For";
  case TokenKind::TK_While:
    return "While";
  case TokenKind::TK_Return:
    return "Return";
  case TokenKind::TK_OpenBrace:
    return "OpenBrace";
  case TokenKind::TK_CloseBrace:
    return "CloseBrace";
  case TokenKind::TK_OpenParen:
    return "OpenParen";
  case TokenKind::TK_CloseParen:
    return "CloseParen";
  case TokenKind::TK_Comma:
    return "Comma";
  case TokenKind::TK_Semicolon:
    return "Semicolon";
  case TokenKind::TK_Colon:
    return "Colon";
  case TokenKind::TK_Question:
    return "Question";
  case TokenKind::TK_Add:
    return "Add";
  case TokenKind::TK_AddEq:
    return "AddEq";
  case TokenKind::TK_Subtract:
    return "Subtract";
  case TokenKind::TK_SubtractEq:
    return "SubtractEq";
  case TokenKind::TK_Multiply:
    return "Multiply";
  case TokenKind::TK_MultiplyEq:
    return "MultiplyEq";
  case TokenKind::TK_Divide:
    return "Divide";
  case TokenKind::TK_DivideEq:
    return "DivideEq";
  case TokenKind::TK_Modulus:
    return "Modulus";
  case TokenKind::TK_ModulusEq:
    return "ModulusEq";
  case TokenKind::TK_Assign:
    return "Assign";
  case TokenKind::TK_LessThan:
    return "LessThan";
  case TokenKind::TK_LessThanEq:
    return "LessThanEq";
  case TokenKind::TK_GreaterThan:
    return "GreaterThan";
  case TokenKind::TK_GreaterThanEq:
    return "GreaterThanEq";
  case TokenKind::TK_Equals:
    return "Equals";
  case TokenKind::TK_NotEquals:
    return "NotEquals";
  case TokenKind::TK_ShiftLeft:
    return "ShiftLeft";
  case TokenKind::TK_ShiftLeftEq:
    return "ShiftLeftEq";
  case TokenKind::TK_ShiftRight:
    return "ShiftRight";
  case TokenKind::TK_ShiftRightEq:
    return "ShiftRightEq";
  case TokenKind::TK_And:
    return "And";
  case TokenKind::TK_AndEq:
    return "AndEq";
  case TokenKind::TK_Or:
    return "Or";
  case TokenKind::TK_OrEq:
    return "OrEq";
  case TokenKind::TK_Xor:
    return "Xor";
  case TokenKind::TK_XorEq:
    return "XorEq";
  case TokenKind::TK_LogicalAnd:
    return "LogicalAnd";
  case TokenKind::TK_LogicalOr:
    return "LogicalOr";
  case TokenKind::TK_Period:
    return "Period";
  case TokenKind::TK_Arrow:
    return "Arrow";
  case TokenKind::TK_Increment:
    return "Increment";
  case TokenKind::TK_Decrement:
    return "Decrement";
  case TokenKind::TK_EOF:
    return "EOF";
  case TokenKind::TK_None:
  default:
    return "None";
  }
}

template <typename TStream>
TStream &operator<<(TStream &Stream, TokenKind Kind) {
  return Stream << tokenKindToString(Kind);
}

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
