#pragma once

#include "ParseInterfaces.h"

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

} // anonymous namespace

template <typename TIterator> class TokenLexer : public ITokenLexer {
public:
  TokenLexer(TIterator Begin, TIterator End);
  virtual ~TokenLexer() = default;

  // ITokenLexer impl.
  virtual void lex() override;
  virtual const std::vector<Token> &getTokens() const override;

private:
  Token lexToken();
  Token lexIdentifier();
  Token lexNumber();
  Token lexChar();
  Token lexString();

  TIterator Current, End;
  std::vector<Token> Tokens;
};

template <typename TIterator>
TokenLexer<TIterator>::TokenLexer(TIterator Begin, TIterator End)
    : Current(Begin), End(End) {}

template <typename TIterator> void TokenLexer<TIterator>::lex() {
  while (Current != End) {
    Tokens.push_back(lexToken());
  }

  Tokens.emplace_back(TokenKind::TK_EOF);
}

template <typename TIterator> Token TokenLexer<TIterator>::lexToken() {
  // Trim any leading whitespace.
  while (Current != End && isspace(*Current)) {
    ++Current;
  }

  if (Current == End) {
    std::cout << "Lexed EOF.\n";
    return Token(TokenKind::TK_EOF);
  }

  if (isalpha(*Current)) {
    return lexIdentifier();
  }

  if (isdigit(*Current)) {
    return lexNumber();
  }

  if (isSymbol(*Current)) {
    std::cout << "Lexed symbol with value " << *Current << ".\n";
    return Token(TokenKind::TK_Symbol, std::string(1, *Current++));
  }

  switch (*Current) {
  case '\'':
    return lexChar();
  case '\"':
    return lexString();
  default:
    throw ParseException("TokenLexer: Unknown token. Unable to lex.");
  }
}

template <typename TIterator> Token TokenLexer<TIterator>::lexIdentifier() {
  std::string Identifier;
  while (Current != End && !isspace(*Current) && !isSymbol(*Current)) {
    // TODO: Handle other legal characters.
    if (!isalpha(*Current) && *Current != '_') {
      throw ParseException("TokenLexer: Encountered non alphabetical character "
                           "in identifier name.");
    }

    Identifier.push_back(*Current);
    ++Current;
  }

  std::cout << "Lexed identifier with value " << Identifier << ".\n";
  return Token(TokenKind::TK_Identifier, std::move(Identifier));
}

template <typename TIterator> Token TokenLexer<TIterator>::lexNumber() {
  std::string Number;
  while (Current != End && !isspace(*Current) && !isSymbol(*Current)) {
    // TODO: Handle floats.
    if (!isdigit(*Current)) {
      throw ParseException(
          "TokenLexer: Encountered non numeric character in number.");
    }

    Number.push_back(*Current);
    ++Current;
  }

  std::cout << "Lexed number with value " << Number << ".\n";
  return Token(TokenKind::TK_Number, std::move(Number));
}

template <typename TIterator> Token TokenLexer<TIterator>::lexChar() {
  assert(Current != End);
  assert(*Current == '\'');

  // TODO: Do some more error checking instead of crashing on bad code.
  ++Current;
  std::string Char(1, *Current);

  ++Current;
  if (*Current != '\'') {
    throw ParseException("TokenLexer: Encountered character literal with a "
                         "length greater than 1.");
  }

  // Could actually be a keyword at this point. Not sure whether it's worth
  // having separate token kinds for each keyword. Or perhaps I could just hoist
  // this responsibility onto the parsing logic.
  ++Current;
  std::cout << "Lexed character with value " << Char << ".\n";
  return Token(TokenKind::TK_Char, std::move(Char));
}

template <typename TIterator> Token TokenLexer<TIterator>::lexString() {
  assert(Current != End);
  assert(*Current == '\"');

  ++Current;

  std::string Literal;
  while (Current != End && *Current != '\"') {
    Literal.push_back(*Current);
    ++Current;
  }

  if (Current == End) {
    throw ParseException(
        "TokenLexer: String literal has no closing quotation mark.");
  }

  assert(*Current == '\"');
  ++Current;

  std::cout << "Lexed string literal with value " << Literal << ".\n";
  return Token(TokenKind::TK_String, std::move(Literal));
}

template <typename TIterator>
const std::vector<Token> &TokenLexer<TIterator>::getTokens() const {
  return Tokens;
}

} // namespace fantac::parse
