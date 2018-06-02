#pragma once

#include "ParseInterfaces.h"

#include <cassert>
#include <iostream>

namespace fantac::parse {

template <typename Iterator> class TokenLexer : public ITokenLexer {
public:
  TokenLexer(Iterator Begin, Iterator End);
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

  Iterator Current, End;
  std::vector<Token> Tokens;
};

template <typename Iterator>
TokenLexer<Iterator>::TokenLexer(Iterator Begin, Iterator End)
    : Current(Begin), End(End) {}

template <typename Iterator> void TokenLexer<Iterator>::lex() {
  while (Current != End) {
    Tokens.push_back(lexToken());
  }

  Tokens.emplace_back(TK_EOF);
}

template <typename Iterator> Token TokenLexer<Iterator>::lexToken() {
  // Trim any leading whitespace.
  while (Current != End && isspace(*Current)) {
    ++Current;
  }

  if (Current == End) {
    return Token(TK_EOF);
  }

  if (isalpha(*Current)) {
    return lexIdentifier();
  }

  if (isdigit(*Current)) {
    return lexNumber();
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

template <typename Iterator> Token TokenLexer<Iterator>::lexIdentifier() {
  std::string Identifier;
  while (Current != End && !isspace(*Current)) {
    // TODO: Handle other legal characters.
    if (!isalpha(*Current) && *Current != '_') {
      throw ParseException("TokenLexer: Encountered non alphabetical character "
                           "in identifier name.");
    }

    Identifier.push_back(*Current);
    ++Current;
  }

  std::cout << "Lexed identifier with value " << Identifier << ".\n";
  return Token(TK_Identifier, std::move(Identifier));
}

template <typename Iterator> Token TokenLexer<Iterator>::lexNumber() {
  std::string Number;
  while (Current != End && !isspace(*Current)) {
    // TODO: Handle floats.
    if (!isdigit(*Current)) {
      throw ParseException(
          "TokenLexer: Encountered non numeric character in number.");
    }

    Number.push_back(*Current);
    ++Current;
  }

  std::cout << "Lexed number with value " << Number << ".\n";
  return Token(TK_Number, std::move(Number));
}

template <typename Iterator> Token TokenLexer<Iterator>::lexChar() {
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

  ++Current;
  std::cout << "Lexed character with value " << Char << ".\n";
  return Token(TK_Char, std::move(Char));
}

template <typename Iterator> Token TokenLexer<Iterator>::lexString() {
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
  return Token(TK_String, std::move(Literal));
}

template <typename Iterator>
const std::vector<Token> &TokenLexer<Iterator>::getTokens() const {
  return Tokens;
}

} // namespace fantac::parse
