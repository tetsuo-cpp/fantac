#include "Parser.h"

#include <algorithm>
#include <cassert>

namespace fantac::parse {

namespace {

const std::vector<std::pair<std::string, ast::CTypeKind>> CTypeMappings = {
    {"int", ast::CTypeKind::CTK_Int},
    {"long", ast::CTypeKind::CTK_Long},
    {"float", ast::CTypeKind::CTK_Float},
    {"double", ast::CTypeKind::CTK_Double},
    {"char", ast::CTypeKind::CTK_Char}};

ast::CTypeKind stringToCTypeKind(const std::string &Type) {
  const auto CTypeMappingIter = std::find_if(
      CTypeMappings.begin(), CTypeMappings.end(),
      [&Type](const std::pair<std::string, ast::CTypeKind> &CTypePair) {
        return Type == CTypePair.first;
      });

  if (CTypeMappingIter == CTypeMappings.end()) {
    return ast::CTypeKind::CTK_None;
  }

  return CTypeMappingIter->second;
}

} // anonymous namespace

Parser::Parser(ILexer &Lexer, util::LoggerFactory &LF)
    : Lexer(Lexer), Logger(LF.createLogger("Parser")) {
  Lexer.lex(CurrentToken);
}

ast::ASTPtr Parser::parseTopLevelExpr() {
  Logger.info("Parsing top level expression.");
  if (CurrentToken.Kind == TokenKind::TK_EOF) {
    Logger.info("Encountered end of file. Stopping parsing.");
    return nullptr;
  }

  const auto Type = stringToCTypeKind(CurrentToken.Value);
  if (Type == ast::CTypeKind::CTK_None) {
    std::string Error(
        fmt::format("Unknown type name: {}.", CurrentToken.Value));
    Logger.error(Error);
    throw ParseException(std::move(Error));
  }

  readNextToken();
  const auto Name = CurrentToken.Value;

  // Function call.
  readNextToken();
  if (consumeToken(TokenKind::TK_OpenParen)) {
    return parseFunction(Type, Name);
  }

  return nullptr;
}

void Parser::readNextToken() {
  if (!Lexer.lex(CurrentToken)) {
    throw ParseException("Unexpected end of token stream.");
  }
}

void Parser::expectToken(TokenKind ExpectedKind) {
  if (!consumeToken(ExpectedKind)) {
    throw ParseException("Failed token expectation.");
  }
}

bool Parser::consumeToken(TokenKind ExpectedKind) {
  if (CurrentToken.Kind == TokenKind::TK_EOF ||
      CurrentToken.Kind != ExpectedKind) {
    return false;
  }

  Lexer.lex(CurrentToken);
  return true;
}

ast::ASTPtr Parser::parseFunction(ast::CTypeKind ReturnType,
                                  const std::string &FunctionName) {
  // Parse arguments.
  std::vector<std::pair<std::string, ast::CTypeKind>> Args;
  while (!consumeToken(TokenKind::TK_CloseParen)) {
    // Skip commas between arguments.
    if (!Args.empty()) {
      expectToken(TokenKind::TK_Comma);
    }

    // Argument type.
    const auto ArgType = stringToCTypeKind(CurrentToken.Value);
    expectToken(TokenKind::TK_Identifier);

    // Argument name.
    Args.emplace_back(CurrentToken.Value, ArgType);
    expectToken(TokenKind::TK_Identifier);
  }

  auto Decl = std::make_unique<ast::FunctionDecl>(FunctionName, ReturnType,
                                                  std::move(Args));

  // Function declaration.
  if (consumeToken(TokenKind::TK_Semicolon)) {
    Logger.info("Found function declaration.");
    return Decl;
  }

  Logger.info("Found function definition.");

  // Otherwise, function definition.
  std::vector<ast::ASTPtr> Body;
  while (!consumeToken(TokenKind::TK_CloseBrace)) {
    Body.push_back(parseStatement());
  }

  return std::make_unique<ast::FunctionDef>(std::move(Decl), std::move(Body));
}

ast::ASTPtr Parser::parseStatement() {
  Logger.debug("Skipping over statement.");

  while (!consumeToken(TokenKind::TK_Semicolon)) {
    readNextToken();
  }

  return nullptr;
}

} // namespace fantac::parse
