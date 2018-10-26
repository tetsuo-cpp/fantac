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
    throw ParseException(
        fmt::format("Unknown type name: {}.", CurrentToken.Value));
  }

  expectToken(TokenKind::TK_Identifier);
  auto Name = CurrentToken.Value;

  // Function call.
  expectToken(TokenKind::TK_Identifier);
  if (consumeToken(TokenKind::TK_OpenParen)) {
    return parseFunction(Type, std::move(Name));
  }

  return nullptr;
}

bool Parser::consumeToken(TokenKind Kind) {
  if (CurrentToken.Kind == TokenKind::TK_EOF || CurrentToken.Kind != Kind) {
    return false;
  }

  Lexer.lex(CurrentToken);
  return true;
}

void Parser::expectToken(TokenKind Kind) {
  if (!consumeToken(Kind)) {
    throw ParseException(fmt::format("Encountered unexpected token. Expected "
                                     "kind {} but found kind {} with value {}.",
                                     Kind, CurrentToken.Kind,
                                     CurrentToken.Value));
  }
}

ast::ASTPtr Parser::parseFunction(ast::CTypeKind Return, std::string &&Name) {
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

  auto Decl = std::make_unique<ast::FunctionDecl>(std::move(Name), Return,
                                                  std::move(Args));

  // Function declaration.
  if (consumeToken(TokenKind::TK_Semicolon)) {
    Logger.info("Found function declaration.");
    return Decl;
  }

  Logger.info("Found function definition.");

  // Otherwise, function definition.
  std::vector<ast::ASTPtr> Body;
  expectToken(TokenKind::TK_OpenBrace);
  while (!consumeToken(TokenKind::TK_CloseBrace)) {
    Body.push_back(parseStatement());
  }

  return std::make_unique<ast::FunctionDef>(std::move(Decl), std::move(Body));
}

ast::ASTPtr Parser::parseStatement() {
  Logger.debug("Skipping over statement.");

  const auto Type = stringToCTypeKind(CurrentToken.Value);
  if (Type != ast::CTypeKind::CTK_None) {
    // Consume type.
    expectToken(TokenKind::TK_Identifier);

    // Consume name.
    auto Name = CurrentToken.Value;
    expectToken(TokenKind::TK_Identifier);

    // TODO: Parse assignments.
    while (!consumeToken(TokenKind::TK_Semicolon) && Lexer.lex(CurrentToken)) {
    }

    return std::make_unique<ast::VariableDecl>(Type, std::move(Name));
  }

  // Skip over statement.
  while (!consumeToken(TokenKind::TK_Semicolon) && Lexer.lex(CurrentToken)) {
  }

  return nullptr;
}

} // namespace fantac::parse
