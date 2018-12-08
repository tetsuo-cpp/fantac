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
  case TokenKind::TK_Not:
    return "Not";
  case TokenKind::TK_Period:
    return "Period";
  case TokenKind::TK_Arrow:
    return "Arrow";
  case TokenKind::TK_Increment:
    return "Increment";
  case TokenKind::TK_Decrement:
    return "Decrement";
  case TokenKind::TK_OpenSquareBracket:
    return "OpenSquareBracket";
  case TokenKind::TK_CloseSquareBracket:
    return "CloseSquareBracket";
  case TokenKind::TK_EOF:
    return "EOF";
  case TokenKind::TK_None:
  default:
    return "None";
  }
}

} // namespace fantac::parse
