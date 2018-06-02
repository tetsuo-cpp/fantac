#pragma once

#include "ParseInterfaces.h"

namespace fantac::parse {

class TokenLexer : public ITokenLexer {
public:
  virtual ~TokenLexer() = default;

  template <typename Iterator> void read(Iterator Begin, Iterator End);

  // ITokenLexer impl.
  virtual const std::vector<Token> &getTokens() const override;

private:
  std::vector<Token> Tokens;
};

template <typename Iterator>
void TokenLexer::read(Iterator Begin, Iterator End) {
  while (Begin != End) {
  }

  Tokens.emplace_back(TK_Eof);
}

const std::vector<Token> &TokenLexer::getTokens() const { return Tokens; }

} // namespace fantac::parse
