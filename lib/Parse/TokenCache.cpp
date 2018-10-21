#include "TokenCache.h"

namespace fantac::parse {

TokenCache::TokenCache(ILexer &Lexer, util::LoggerFactory &LF)
    : Lexer(Lexer), Logger(LF.createLogger("TokenCache")) {}

Token &TokenCache::getCurrentToken() { return Tok; }

void TokenCache::readNextToken() {
  if (!Cache.empty()) {
    Tok = Cache.front();
    Cache.pop_front();
    return;
  }

  if (!Lexer.lex(Tok)) {
    std::string Error("Unexpected end of token stream.");
    Logger.error(Error);
    throw ParseException(std::move(Error));
  }
}

bool TokenCache::readAndCacheNextToken() {
  if (CacheIndex < Cache.size()) {
    Tok = Cache.at(CacheIndex++);
    return true;
  } else {
    if (!Lexer.lex(Tok)) {
      return false;
    }

    Cache.push_back(Tok);
    ++CacheIndex;
    return true;
  }
}

void TokenCache::resetCache() { CacheIndex = 0; }

void TokenCache::clearCache() { Cache.clear(); }

} // namespace fantac::parse
