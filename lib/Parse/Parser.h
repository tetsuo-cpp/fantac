#pragma once

#include "ParseInterfaces.h"
#include "TokenCache.h"

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
  void expectToken(TokenKind ExpectedKind);
  bool consumeToken(TokenKind ExpectedKind);
  ast::ASTPtr parseFunction(ast::CTypeKind ReturnType,
                            const std::string &FunctionName);
  ast::ASTPtr parseStatement();

  ILexer &Lexer;
  Token CurrentToken;
  spdlog::logger &Logger;
};

} // namespace fantac::parse
