#pragma once

#include "ParseInterfaces.h"

#include <Util/LoggerFactory.h>

#include <deque>

namespace fantac::parse {

class TokenCache {
public:
  TokenCache(ILexer &Lexer, util::LoggerFactory &LF);
  virtual ~TokenCache() = default;

  Token &getCurrentToken();
  void readNextToken();
  bool readAndCacheNextToken();
  void resetCache();
  void clearCache();

private:
  ILexer &Lexer;
  Token Tok;
  std::deque<Token> Cache;
  unsigned int CacheIndex;
  spdlog::logger &Logger;
};

} // namespace fantac::parse
