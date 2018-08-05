#include "Lexer.h"

#include <cassert>
#include <iostream>

namespace fantac::parse {

namespace {

bool isSymbol(char Char) {
  switch (Char) {
  case '+':
  case '-':
  case '*':
  case '/':
  case '(':
  case ')':
  case '{':
  case '}':
  case ';':
  case ':':
  case '<':
  case '>':
  case '=':
  case ',':
    return true;
  default:
    return false;
  }
}

} // namespace

Lexer::Lexer(const char *Begin, const char *End)
    : CurrentChar(*Begin), Current(Begin + 1), End(End) {}

bool Lexer::lex(Token &Tok) {
  if (!lexToken(Tok)) {
    return false;
  }

  return Current != End;
}

bool Lexer::lexToken(Token &Tok) {
  // Trim any leading whitespace.
  while (isspace(CurrentChar)) {
    if (!readNextChar()) {
      std::cout << "Lexed EOF.\n";
      Tok.assign(TokenKind::TK_EOF);
      return false;
    }
  }

  if (isalpha(CurrentChar)) {
    lexIdentifier(Tok);
    return true;
  }

  if (isdigit(CurrentChar)) {
    lexNumber(Tok);
    return true;
  }

  if (isSymbol(CurrentChar)) {
    std::cout << "Lexed symbol with value " << CurrentChar << ".\n";
    Tok.assign(TokenKind::TK_Symbol, std::string(1, CurrentChar));
    readNextChar();
    return true;
  }

  switch (CurrentChar) {
  case '\'':
    lexChar(Tok);
  case '\"':
    lexString(Tok);
  default:
    throw ParseException("Lexer: Unknown token. Unable to lex.");
  }

  readNextChar();
  return true;
}

void Lexer::lexIdentifier(Token &Tok) {
  std::string Identifier;
  while (!isspace(CurrentChar) && !isSymbol(CurrentChar)) {
    if (!isalpha(CurrentChar) && CurrentChar != '_') {
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
  while (!isspace(CurrentChar) && !isSymbol(CurrentChar)) {
    if (!isdigit(CurrentChar)) {
      throw ParseException(
          "Lexer: Encountered non numeric character in number.");
    }

    NumberLiteral.push_back(CurrentChar);
    if (!readNextChar()) {
      break;
    }
  }

  std::cout << "Lexed number with value " << NumberLiteral << ".\n";
  Tok.assign(TokenKind::TK_Number, std::move(NumberLiteral));
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
  Tok.assign(TokenKind::TK_Char, std::move(CharLiteral));
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
  Tok.assign(TokenKind::TK_String, std::move(StringLiteral));
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
