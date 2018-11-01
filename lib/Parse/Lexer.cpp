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
    {'>', TokenKind::TK_GreaterThan}, {'=', TokenKind::TK_Assign},
    {'.', TokenKind::TK_Period},      {'%', TokenKind::TK_Modulus},
    {'&', TokenKind::TK_And},         {'|', TokenKind::TK_Or},
    {'^', TokenKind::TK_Xor},         {'!', TokenKind::TK_Not}};

const std::vector<std::pair<std::string, TokenKind>> CompoundSymbolMappings = {
    {"+=", TokenKind::TK_AddEq},         {"-=", TokenKind::TK_SubtractEq},
    {"*=", TokenKind::TK_MultiplyEq},    {"/=", TokenKind::TK_DivideEq},
    {"%=", TokenKind::TK_ModulusEq},     {"==", TokenKind::TK_Equals},
    {"<=", TokenKind::TK_LessThanEq},    {">=", TokenKind::TK_GreaterThanEq},
    {"!=", TokenKind::TK_NotEquals},     {"<<", TokenKind::TK_ShiftLeft},
    {"<<=", TokenKind::TK_ShiftLeftEq},  {">>", TokenKind::TK_ShiftRight},
    {">>=", TokenKind::TK_ShiftRightEq}, {"&=", TokenKind::TK_AndEq},
    {"|=", TokenKind::TK_OrEq},          {"^=", TokenKind::TK_XorEq},
    {"&&", TokenKind::TK_LogicalAnd},    {"||", TokenKind::TK_LogicalOr}};

const std::vector<std::pair<std::string, TokenKind>> KeywordMappings = {
    {"if", TokenKind::TK_If},         {"else", TokenKind::TK_Else},
    {"for", TokenKind::TK_For},       {"while", TokenKind::TK_While},
    {"return", TokenKind::TK_Return}, {"sizeof", TokenKind::TK_SizeOf}};

std::pair<bool, TokenKind> isSymbol(char Char) {
  const auto SymbolMappingIter =
      std::find_if(SymbolMappings.begin(), SymbolMappings.end(),
                   [Char](const std::pair<char, TokenKind> &SymbolPair) {
                     return Char == SymbolPair.first;
                   });

  if (SymbolMappingIter == SymbolMappings.end()) {
    return std::make_pair(false, TokenKind::TK_None);
  }

  return std::make_pair(true, SymbolMappingIter->second);
}

std::pair<bool, TokenKind> isCompoundSymbol(const std::string &Value) {
  const auto CSymbolMappingIter = std::find_if(
      CompoundSymbolMappings.begin(), CompoundSymbolMappings.end(),
      [&Value](const std::pair<std::string, TokenKind> &CSymbolPair) {
        return Value == CSymbolPair.first;
      });

  if (CSymbolMappingIter == CompoundSymbolMappings.end()) {
    return std::make_pair(false, TokenKind::TK_None);
  }

  return std::make_pair(true, CSymbolMappingIter->second);
}

} // namespace

Lexer::Lexer(const char *Begin, const char *End, util::LoggerFactory &LF)
    : CurrentChar(*Begin), Current(Begin + 1), End(End),
      Logger(LF.createLogger("Lexer")) {
  assert(Begin < End);
}

bool Lexer::lex(Token &Tok) {
  if (Current > End) {
    Tok.assign(TokenKind::TK_EOF);
    return false;
  }

  return lexToken(Tok);
}

bool Lexer::lexToken(Token &Tok) {
  // Trim any leading whitespace.
  while (std::isspace(CurrentChar)) {
    if (!readNextChar()) {
      Logger->info("Lexed EOF. Finished lexing.");
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
    auto Kind = Result.second;
    std::string CompoundSymbol{CurrentChar};
    while (readNextChar()) {
      if (std::isspace(CurrentChar)) {
        continue;
      }

      CompoundSymbol.push_back(CurrentChar);
      const auto CResult = isCompoundSymbol(CompoundSymbol);
      if (!CResult.first) {
        CompoundSymbol.pop_back();
        break;
      } else {
        Kind = CResult.second;
      }
    }

    Logger->debug("Lexed symbol with kind {} and value {}.", Kind,
                  CompoundSymbol);
    Tok.assign(Kind, std::move(CompoundSymbol));
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
    throw ParseException("Unknown token. Unable to lex.");
  }

  readNextChar();
  return true;
}

void Lexer::lexIdentifier(Token &Tok) {
  std::string Identifier;
  while (!std::isspace(CurrentChar) && !isSymbol(CurrentChar).first) {
    if (!std::isalpha(CurrentChar) && CurrentChar != '_') {
      throw ParseException(
          "Encountered non-alphabetical character in identifier name.");
    }

    Identifier.push_back(CurrentChar);
    if (!readNextChar()) {
      break;
    }
  }

  auto KeywordIter = std::find_if(
      KeywordMappings.begin(), KeywordMappings.end(),
      [&Identifier](const std::pair<std::string, TokenKind> &KeywordPair) {
        return Identifier == KeywordPair.first;
      });

  // Recognised a keyword.
  if (KeywordIter != KeywordMappings.end()) {
    Logger->debug("Lexed keyword of kind {} and value {}.", KeywordIter->second,
                  Identifier);
    Tok.assign(KeywordIter->second, std::move(Identifier));
    return;
  }

  // Otherwise it's just an identifier.
  Logger->debug("Lexed identifier with value {}.", Identifier);
  Tok.assign(TokenKind::TK_Identifier, std::move(Identifier));
}

void Lexer::lexNumber(Token &Tok) {
  std::string NumberLiteral;
  while (!std::isspace(CurrentChar) && !isSymbol(CurrentChar).first) {
    if (!std::isdigit(CurrentChar)) {
      throw ParseException("Encountered non-numeric character in number.");
    }

    NumberLiteral.push_back(CurrentChar);
    if (!readNextChar()) {
      break;
    }
  }

  Logger->debug("Lexed number with value {}.", NumberLiteral);
  Tok.assign(TokenKind::TK_NumberLiteral, std::move(NumberLiteral));
}

void Lexer::lexChar(Token &Tok) {
  assert(CurrentChar == '\'');

  if (!readNextChar()) {
    throw ParseException(
        "Encountered opening single quote at the end of source.");
  }

  std::string CharLiteral(1, CurrentChar);

  if (!readNextChar() && CurrentChar != '\'') {
    throw ParseException(
        "Encountered character literal with a length greater than 1.");
  }

  Logger->debug("Lexed character with value {}.", CharLiteral);
  Tok.assign(TokenKind::TK_CharLiteral, std::move(CharLiteral));
}

void Lexer::lexString(Token &Tok) {
  assert(CurrentChar == '\"');

  if (!readNextChar()) {
    throw ParseException(
        "Encountered open quotation mark at the end of source.");
  }

  std::string StringLiteral;
  while (CurrentChar != '\"') {
    StringLiteral.push_back(CurrentChar);
    if (!readNextChar()) {
      throw ParseException("String literal has no closing quotation mark.");
    }
  }

  assert(CurrentChar == '\"');

  Logger->debug("Lexed string literal with value {}.", StringLiteral);
  Tok.assign(TokenKind::TK_StringLiteral, std::move(StringLiteral));
}

bool Lexer::readNextChar() {
  if (Current > End) {
    return false;
  }

  // Read and advance to next char.
  CurrentChar = *Current;
  ++Current;
  return true;
}

} // namespace fantac::parse
