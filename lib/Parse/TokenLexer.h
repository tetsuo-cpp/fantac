#pragma once

namespace fantac::parse {

class TokenLexer {
public:
  virtual ~TokenLexer() = default;

  template <typename Iterator> void read(Iterator Begin, Iterator End);

private:
};

template <typename Iterator>
void TokenLexer::read(Iterator Begin, Iterator End) {
  static_cast<void>(Begin);
  static_cast<void>(End);
}

} // namespace fantac::parse
