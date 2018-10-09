#include "Lexer.h"

#include <algorithm>
#include <cassert>
#include <iostream>

namespace fantac::parse {

namespace {

const std::vector<std::pair<char, TokenKind>> SymbolMappings = {
    {'{', TokenKind::TK_OpenBrace},   {'}', TokenKind::TK_CloseBrace},
    {'(', TokenKind::TK_OpenParen},   {')', TokenKind::TK_CloseParen},
    {',', TokenKind::TK_Comma},       {';', TokenKind::TK_Semicolon},
    {':', TokenKind::TK_Colon},       {'+', TokenKind::TK_Add},
    {'-', TokenKind::TK_Subtract},    {'*', TokenKind::TK_Multiply},
    {'/', TokenKind::TK_Divide},      {'<', TokenKind::TK_LessThan},
    {'>', TokenKind::TK_GreaterThan}, {'=', TokenKind::TK_Equals}};

std::pair<bool, TokenKind> isSymbol(char Char) {
  const auto SymbolMappingIter =
      std::find_if(SymbolMappings.begin(), SymbolMappings.end(),
                   [Char](const std::pair<char, TokenKind> &SymbolPair) {
                     return Char == SymbolPair.first;
                   });

  if (SymbolMappingIter == SymbolMappings.end()) {
    return std::make_pair(false, TokenKind::TK_EOF);
  }

  return std::make_pair(true, SymbolMappingIter->second);
}

} // namespace

Lexer::Lexer(const char *Begin, const char *End)
    : CurrentChar(*Begin), Current(Begin + 1), End(End) {
  assert(Begin != End);
}

bool Lexer::lex(Token &Tok) {
  if (Current == End) {
    Tok.assign(TokenKind::TK_EOF);
    return false;
  }

  return lexToken(Tok);
}

bool Lexer::lexToken(Token &Tok) {
  // Trim any leading whitespace.
  while (std::isspace(CurrentChar)) {
    if (!readNextChar()) {
      std::cout << "Lexed EOF.\n";
      Tok.assign(TokenKind::TK_EOF);
      return false;
    }
  }

  if (std::isalpha(CurrentChar)) {
    lexIdentifier(Tok);
    return true;
  }

  if (std::isdigit(CurrentChar)) {
    lexNumber(Tok);
    return true;
  }

  const auto Result = isSymbol(CurrentChar);
  if (Result.first) {
    std::cout << "Lexed symbol with value " << CurrentChar << ".\n";
    Tok.assign(Result.second, std::string(1, CurrentChar));
    readNextChar();
    return true;
  }

  switch (CurrentChar) {
  case '\'':
    lexChar(Tok);
    break;
  case '\"':
    lexString(Tok);
    break;
  default:
    throw ParseException("Lexer: Unknown token. Unable to lex.");
  }

  readNextChar();
  return true;
}

void Lexer::lexIdentifier(Token &Tok) {
  std::string Identifier;
  while (!std::isspace(CurrentChar) && !isSymbol(CurrentChar).first) {
    if (!std::isalpha(CurrentChar) && CurrentChar != '_') {
      throw ParseException("Lexer: Encountered non alphabetical character "
                           "in identifier name.");
    }

    Identifier.push_back(CurrentChar);
    if (!readNextChar()) {
      break;
    }
  }

  std::cout << "Lexed identifier with value " << Identifier << ".\n";
  Tok.assign(TokenKind::TK_Identifier, std::move(Identifier));
}

void Lexer::lexNumber(Token &Tok) {
  std::string NumberLiteral;
  while (!std::isspace(CurrentChar) && !isSymbol(CurrentChar).first) {
    if (!std::isdigit(CurrentChar)) {
      throw ParseException(
          "Lexer: Encountered non numeric character in number.");
    }

    NumberLiteral.push_back(CurrentChar);
    if (!readNextChar()) {
      break;
    }
  }

  std::cout << "Lexed number with value " << NumberLiteral << ".\n";
  Tok.assign(TokenKind::TK_NumberLiteral, std::move(NumberLiteral));
}

void Lexer::lexChar(Token &Tok) {
  assert(CurrentChar == '\'');

  if (!readNextChar()) {
    throw ParseException(
        "Lexer: Encountered opening single quote at the end of source.");
  }

  std::string CharLiteral(1, CurrentChar);

  if (!readNextChar() && CurrentChar != '\'') {
    throw ParseException("Lexer: Encountered character literal with a "
                         "length greater than 1.");
  }

  std::cout << "Lexed character with value " << CharLiteral << ".\n";
  Tok.assign(TokenKind::TK_CharLiteral, std::move(CharLiteral));
}

void Lexer::lexString(Token &Tok) {
  assert(CurrentChar == '\"');

  if (!readNextChar()) {
    throw ParseException(
        "Lexer: Encountered open quotation mark at the end of source.");
  }

  std::string StringLiteral;
  while (CurrentChar != '\"') {
    StringLiteral.push_back(CurrentChar);
    if (!readNextChar()) {
      throw ParseException(
          "Lexer: String literal has no closing quotation mark.");
    }
  }

  assert(CurrentChar == '\"');

  std::cout << "Lexed string literal with value " << StringLiteral << ".\n";
  Tok.assign(TokenKind::TK_StringLiteral, std::move(StringLiteral));
}

bool Lexer::readNextChar() {
  if (Current == End) {
    return false;
  }

  // Read and advance to next char.
  CurrentChar = *Current;
  ++Current;
  return true;
}

} // namespace fantac::parse
