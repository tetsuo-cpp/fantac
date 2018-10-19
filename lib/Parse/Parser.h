#pragma once

#include "ParseInterfaces.h"

#include <AST/AST.h>
#include <Util/LoggerFactory.h>

#include <deque>

namespace fantac::parse {

class Parser {
public:
  Parser(ILexer &Lexer, util::LoggerFactory &LF);
  virtual ~Parser() = default;

  ast::ASTPtr parseTopLevelExpr();

private:
  void readNextToken();
  bool readAndCacheNextToken();
  void resetCache();
  void clearCache();
  void checkNextTokenKind(TokenKind Kind, const std::string &Error);
  bool checkNextCachedTokenKind(TokenKind Kind);
  bool isFunctionSig();
  bool isFunctionDecl();
  bool isFunctionDef();
  bool isVarDecl();
  std::unique_ptr<ast::FunctionDecl> parseFunctionSig();
  ast::ASTPtr parseFunctionDecl();
  ast::ASTPtr parseFunctionDef();
  ast::ASTPtr parseStatement();
  ast::ASTPtr parseVarDecl();

  ILexer &Lexer;
  Token Tok;
  std::deque<Token> TokenCache;
  unsigned int CacheIndex;
  spdlog::logger Logger;
};

} // namespace fantac::parse
