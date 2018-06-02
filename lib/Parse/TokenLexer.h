#pragma once

#include "ParseInterfaces.h"

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
  while (Current != End && isblank(*Current)) {
    ++Current;
  }

  if (isalpha(*Current)) {
    return lexIdentifier();
  }

  if (isdigit(*Current)) {
    return lexNumber();
  }

  return Token(TK_EOF);
}

template <typename Iterator> Token TokenLexer<Iterator>::lexIdentifier() {
  std::string Identifier;
  while (Current != End && !isblank(*Current)) {
    // TODO: Handle underscores and other legal characters.
    if (!isalpha(*Current)) {
      throw ParseException("TokenLexer: Encountered non alphabetical character "
                           "in identifier name.");
    }

    Identifier.push_back(*Current);
    ++Current;
  }

  return Token(TK_Identifier, std::move(Identifier));
}

template <typename Iterator> Token TokenLexer<Iterator>::lexNumber() {
  std::string Number;
  while (Current != End && !isblank(*Current)) {
    // TODO: Handle floats.
    if (!isdigit(*Current)) {
      throw ParseException(
          "TokenLexer: Encountered non numeric character in number.");
    }

    Number.push_back(*Current);
    ++Current;
  }

  return Token(TK_Number, std::move(Number));
}

template <typename Iterator>
const std::vector<Token> &TokenLexer<Iterator>::getTokens() const {
  return Tokens;
}

} // namespace fantac::parse
