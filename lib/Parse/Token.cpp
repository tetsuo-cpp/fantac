#include "Token.h"

namespace fantac::parse {

void Token::assign(TokenKind Kind) {
  this->Kind = Kind;
  Value.clear();
}

std::string tokenKindToString(TokenKind Kind) {
  switch (Kind) {
  case TokenKind::TK_Identifier:
    return "Identifier";
  case TokenKind::TK_Symbol:
    return "Symbol";
  case TokenKind::TK_IntegerLiteral:
    return "IntegerLiteral";
  case TokenKind::TK_FloatLiteral:
    return "FloatLiteral";
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
  case TokenKind::TK_SizeOf:
    return "SizeOf";
  case TokenKind::TK_OpenBrace:
    return "OpenBrace";
  case TokenKind::TK_CloseBrace:
    return "CloseBrace";
  case TokenKind::TK_OpenParen:
    return "OpenParen";
  case TokenKind::TK_CloseParen:
    return "CloseParen";
  case TokenKind::TK_Comma:
    return ",";
  case TokenKind::TK_Semicolon:
    return "Semicolon";
  case TokenKind::TK_Colon:
    return "Colon";
  case TokenKind::TK_Question:
    return "?";
  case TokenKind::TK_Add:
    return "+";
  case TokenKind::TK_AddEq:
    return "+=";
  case TokenKind::TK_Subtract:
    return "-";
  case TokenKind::TK_SubtractEq:
    return "-=";
  case TokenKind::TK_Multiply:
    return "*";
  case TokenKind::TK_MultiplyEq:
    return "*=";
  case TokenKind::TK_Divide:
    return "/";
  case TokenKind::TK_DivideEq:
    return "/=";
  case TokenKind::TK_Modulus:
    return "%";
  case TokenKind::TK_ModulusEq:
    return "%=";
  case TokenKind::TK_Assign:
    return "=";
  case TokenKind::TK_LessThan:
    return "<";
  case TokenKind::TK_LessThanEq:
    return "<=";
  case TokenKind::TK_GreaterThan:
    return ">";
  case TokenKind::TK_GreaterThanEq:
    return ">=";
  case TokenKind::TK_Equals:
    return "==";
  case TokenKind::TK_NotEquals:
    return "!=";
  case TokenKind::TK_ShiftLeft:
    return "<<";
  case TokenKind::TK_ShiftLeftEq:
    return "<<=";
  case TokenKind::TK_ShiftRight:
    return ">>";
  case TokenKind::TK_ShiftRightEq:
    return ">>=";
  case TokenKind::TK_And:
    return "&";
  case TokenKind::TK_AndEq:
    return "&=";
  case TokenKind::TK_Or:
    return "|";
  case TokenKind::TK_OrEq:
    return "|=";
  case TokenKind::TK_Xor:
    return "^";
  case TokenKind::TK_XorEq:
    return "^=";
  case TokenKind::TK_LogicalAnd:
    return "&&";
  case TokenKind::TK_LogicalOr:
    return "||";
  case TokenKind::TK_Not:
    return "!";
  case TokenKind::TK_Period:
    return ".";
  case TokenKind::TK_Arrow:
    return "->";
  case TokenKind::TK_Increment:
    return "++";
  case TokenKind::TK_Decrement:
    return "--";
  case TokenKind::TK_OpenSquareBracket:
    return "OpenSquareBracket";
  case TokenKind::TK_CloseSquareBracket:
    return "CloseSquareBracket";
  case TokenKind::TK_SingleLineComment:
    return "SingleLineComment";
  case TokenKind::TK_Hash:
    return "Hash";
  case TokenKind::TK_Void:
    return "Void";
  case TokenKind::TK_Char:
    return "Char";
  case TokenKind::TK_Int:
    return "Int";
  case TokenKind::TK_Float:
    return "Float";
  case TokenKind::TK_Double:
    return "Double";
  case TokenKind::TK_Unsigned:
    return "Unsigned";
  case TokenKind::TK_Short:
    return "Short";
  case TokenKind::TK_Long:
    return "Long";
  case TokenKind::TK_Enum:
    return "Enum";
  case TokenKind::TK_Struct:
    return "Struct";
  case TokenKind::TK_EOF:
    return "EOF";
  case TokenKind::TK_None:
  default:
    return "None";
  }
}

} // namespace fantac::parse
