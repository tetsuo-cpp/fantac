#include "Lexer.h"
#include "Token.h"

#include <cassert>
#include <vector>

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
    {'^', TokenKind::TK_Xor},         {'!', TokenKind::TK_Not},
    {'#', TokenKind::TK_Hash}};

const std::vector<std::pair<std::string, TokenKind>> CompoundSymbolMappings = {
    {"+=", TokenKind::TK_AddEq},
    {"-=", TokenKind::TK_SubtractEq},
    {"*=", TokenKind::TK_MultiplyEq},
    {"/=", TokenKind::TK_DivideEq},
    {"%=", TokenKind::TK_ModulusEq},
    {"==", TokenKind::TK_Equals},
    {"<=", TokenKind::TK_LessThanEq},
    {">=", TokenKind::TK_GreaterThanEq},
    {"!=", TokenKind::TK_NotEquals},
    {"<<", TokenKind::TK_ShiftLeft},
    {"<<=", TokenKind::TK_ShiftLeftEq},
    {">>", TokenKind::TK_ShiftRight},
    {">>=", TokenKind::TK_ShiftRightEq},
    {"&=", TokenKind::TK_AndEq},
    {"|=", TokenKind::TK_OrEq},
    {"^=", TokenKind::TK_XorEq},
    {"&&", TokenKind::TK_LogicalAnd},
    {"||", TokenKind::TK_LogicalOr},
    {"->", TokenKind::TK_Arrow},
    {"//", TokenKind::TK_SingleLineComment}};

const std::vector<std::pair<std::string, TokenKind>> KeywordMappings = {
    {"if", TokenKind::TK_If},         {"else", TokenKind::TK_Else},
    {"for", TokenKind::TK_For},       {"while", TokenKind::TK_While},
    {"return", TokenKind::TK_Return}, {"sizeof", TokenKind::TK_SizeOf},
    {"void", TokenKind::TK_Void},     {"char", TokenKind::TK_Char},
    {"int", TokenKind::TK_Int},       {"float", TokenKind::TK_Float},
    {"double", TokenKind::TK_Double}, {"unsigned", TokenKind::TK_Unsigned},
    {"short", TokenKind::TK_Short},   {"long", TokenKind::TK_Long},
    {"enum", TokenKind::TK_Enum},     {"struct", TokenKind::TK_Struct}};

template <typename T>
std::pair<bool, TokenKind>
isSymbol(const T &Value, const std::vector<std::pair<T, TokenKind>> &Mappings) {
  const auto MappingIter =
      std::find_if(Mappings.begin(), Mappings.end(),
                   [&Value](const std::pair<T, TokenKind> &SymbolPair) {
                     return Value == SymbolPair.first;
                   });

  if (MappingIter == Mappings.end())
    return std::make_pair(false, TokenKind::TK_None);

  return std::make_pair(true, MappingIter->second);
}

} // namespace

Lexer::Lexer(const char *Begin, const char *End)
    : CurrentChar(*Begin), Current(Begin + 1), End(End) {
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
  while (std::isspace(CurrentChar))
    if (!readNextChar()) {
      Tok.assign(TokenKind::TK_EOF);
      return false;
    }

  if (std::isalpha(CurrentChar)) {
    lexIdentifier(Tok);
    return true;
  }

  if (std::isdigit(CurrentChar)) {
    lexNumber(Tok);
    return true;
  }

  const auto Result = isSymbol(CurrentChar, SymbolMappings);
  if (Result.first) {
    auto Kind = Result.second;
    std::string CompoundSymbol{CurrentChar};
    while (readNextChar()) {
      if (std::isspace(CurrentChar))
        continue;

      CompoundSymbol.push_back(CurrentChar);
      const auto CResult = isSymbol(CompoundSymbol, CompoundSymbolMappings);
      if (!CResult.first) {
        CompoundSymbol.pop_back();
        break;
      } else {
        Kind = CResult.second;
      }
    }

    // TODO: Implement preprocessor. Until then just ignore.
    if (Kind == TokenKind::TK_SingleLineComment || Kind == TokenKind::TK_Hash) {
      while (readNextChar() && CurrentChar != '\n') {
      }
      return lexToken(Tok);
    }

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
  while (!std::isspace(CurrentChar) &&
         !isSymbol(CurrentChar, SymbolMappings).first) {
    if (!std::isalpha(CurrentChar) && CurrentChar != '_')
      throw ParseException(
          "Encountered non-alphabetical character in identifier name.");

    Identifier.push_back(CurrentChar);
    if (!readNextChar())
      break;
  }

  auto KeywordIter = std::find_if(
      KeywordMappings.begin(), KeywordMappings.end(),
      [&Identifier](const std::pair<std::string, TokenKind> &KeywordPair) {
        return Identifier == KeywordPair.first;
      });

  // Recognised a keyword.
  if (KeywordIter != KeywordMappings.end()) {
    Tok.assign(KeywordIter->second, std::move(Identifier));
    return;
  }

  // Otherwise it's just an identifier.
  Tok.assign(TokenKind::TK_Identifier, std::move(Identifier));
}

void Lexer::lexNumber(Token &Tok) {
  std::string NumberLiteral;
  while (!std::isspace(CurrentChar) &&
         !isSymbol(CurrentChar, SymbolMappings).first) {
    if (!std::isdigit(CurrentChar))
      throw ParseException("Encountered non-numeric character in number.");

    NumberLiteral.push_back(CurrentChar);
    if (!readNextChar())
      break;
  }

  // In the case of decimal, read after the period.
  if (CurrentChar == '.') {
    NumberLiteral.push_back(CurrentChar);
    while (readNextChar() && !isSymbol(CurrentChar, SymbolMappings).first)
      NumberLiteral.push_back(CurrentChar);

    Tok.assign(TokenKind::TK_FloatLiteral, std::move(NumberLiteral));
  } else {
    Tok.assign(TokenKind::TK_IntegerLiteral, std::move(NumberLiteral));
  }
}

void Lexer::lexChar(Token &Tok) {
  assert(CurrentChar == '\'');

  if (!readNextChar())
    throw ParseException(
        "Encountered opening single quote at the end of source.");

  std::string CharLiteral{CurrentChar};

  if (!readNextChar() && CurrentChar != '\'')
    throw ParseException(
        "Encountered character literal with a length greater than 1.");

  Tok.assign(TokenKind::TK_CharLiteral, std::move(CharLiteral));
}

void Lexer::lexString(Token &Tok) {
  assert(CurrentChar == '\"');

  if (!readNextChar())
    throw ParseException(
        "Encountered open quotation mark at the end of source.");

  std::string StringLiteral;
  while (CurrentChar != '\"') {
    StringLiteral.push_back(CurrentChar);
    if (!readNextChar())
      throw ParseException("String literal has no closing quotation mark.");
  }

  assert(CurrentChar == '\"');

  Tok.assign(TokenKind::TK_StringLiteral, std::move(StringLiteral));
}

bool Lexer::readNextChar() {
  if (Current > End)
    return false;

  // Read and advance to next char.
  CurrentChar = *Current;
  ++Current;
  return true;
}

} // namespace fantac::parse
