#pragma once

#include <fmt/format.h>

#include <string>

namespace fantac::parse {

enum class TokenKind {
  TK_Identifier,
  TK_Symbol,
  // Literals.
  TK_IntegerLiteral,
  TK_FloatLiteral,
  TK_CharLiteral,
  TK_StringLiteral,
  // Keywords.
  TK_If,
  TK_Else,
  TK_For,
  TK_While,
  TK_Return,
  TK_SizeOf,
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
  TK_Not,
  // Postfix.
  TK_Period,
  TK_Arrow,
  TK_Increment,
  TK_Decrement,
  TK_OpenSquareBracket,
  TK_CloseSquareBracket,
  // Comments.
  TK_SingleLineComment,
  // Preprocessor.
  TK_Hash,
  // Types.
  TK_Void,
  TK_Char,
  TK_Int,
  TK_Float,
  TK_Double,
  TK_Unsigned,
  TK_Short,
  TK_Long,
  TK_Enum,
  TK_Struct,
  // End of file.
  TK_EOF,
  TK_None
};

struct Token {
  template <typename T> void assign(TokenKind Kind, T &&Value) {
    this->Kind = Kind;
    this->Value = std::forward<T>(Value);
  }

  void assign(TokenKind Kind);

  TokenKind Kind = TokenKind::TK_None;
  std::string Value;
};

std::string tokenKindToString(TokenKind Kind);

} // namespace fantac::parse
