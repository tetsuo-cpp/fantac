#include "Lexer.h"

#include <algorithm>
#include <cassert>

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

Lexer::Lexer(const char *Begin, const char *End, util::LoggerFactory &LF)
    : CurrentChar(*Begin), Current(Begin + 1), End(End),
      Logger(LF.createLogger("Lexer")) {
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
      Logger.info("Lexed EOF. Finished lexing.");
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
    Logger.debug("Lexed symbol with value {}.", CurrentChar);
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
    std::string Error("Unknown token. Unable to lex.");
    Logger.error(Error);
    throw ParseException(std::move(Error));
  }

  readNextChar();
  return true;
}

void Lexer::lexIdentifier(Token &Tok) {
  std::string Identifier;
  while (!std::isspace(CurrentChar) && !isSymbol(CurrentChar).first) {
    if (!std::isalpha(CurrentChar) && CurrentChar != '_') {
      std::string Error(
          "Encountered non-alphabetical character in identifier name.");
      Logger.error(Error);
      throw ParseException(std::move(Error));
    }

    Identifier.push_back(CurrentChar);
    if (!readNextChar()) {
      break;
    }
  }

  Logger.debug("Lexed identifier with value {}.", Identifier);
  Tok.assign(TokenKind::TK_Identifier, std::move(Identifier));
}

void Lexer::lexNumber(Token &Tok) {
  std::string NumberLiteral;
  while (!std::isspace(CurrentChar) && !isSymbol(CurrentChar).first) {
    if (!std::isdigit(CurrentChar)) {
      std::string Error("Encountered non-numeric character in number.");
      Logger.error(Error);
      throw ParseException(std::move(Error));
    }

    NumberLiteral.push_back(CurrentChar);
    if (!readNextChar()) {
      break;
    }
  }

  Logger.debug("Lexed number with value {}.", NumberLiteral);
  Tok.assign(TokenKind::TK_NumberLiteral, std::move(NumberLiteral));
}

void Lexer::lexChar(Token &Tok) {
  assert(CurrentChar == '\'');

  if (!readNextChar()) {
    std::string Error("Encountered opening single quote at the end of source.");
    Logger.error(Error);
    throw ParseException(std::move(Error));
  }

  std::string CharLiteral(1, CurrentChar);

  if (!readNextChar() && CurrentChar != '\'') {
    std::string Error(
        "Encountered character literal with a length greater than 1.");
    Logger.error(Error);
    throw ParseException(std::move(Error));
  }

  Logger.debug("Lexed character with value {}.", CharLiteral);
  Tok.assign(TokenKind::TK_CharLiteral, std::move(CharLiteral));
}

void Lexer::lexString(Token &Tok) {
  assert(CurrentChar == '\"');

  if (!readNextChar()) {
    std::string Error("Encountered open quotation mark at the end of source.");
    Logger.error(Error);
    throw ParseException(std::move(Error));
  }

  std::string StringLiteral;
  while (CurrentChar != '\"') {
    StringLiteral.push_back(CurrentChar);
    if (!readNextChar()) {
      std::string Error("String literal has no closing quotation mark.");
      Logger.error(Error);
      throw ParseException(std::move(Error));
    }
  }

  assert(CurrentChar == '\"');

  Logger.debug("Lexed string literal with value {}.", StringLiteral);
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
