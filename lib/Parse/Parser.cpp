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
    : Cache(Lexer, LF), Logger(LF.createLogger("Parser")) {}

ast::ASTPtr Parser::parseTopLevelExpr() {
  Logger.info("Parsing top level expression.");
  Cache.resetCache();
  Cache.clearCache();

  // Find out what top level expr we're currently looking at and then parse it.
  if (isFunctionDecl()) {
    Cache.resetCache();
    Logger.info("Parsing function declaration.");
    return parseFunctionDecl();
  }

  Cache.resetCache();

  if (isFunctionDef()) {
    Cache.resetCache();
    Logger.info("Parsing function definition.");
    return parseFunctionDef();
  }

  if (Cache.getCurrentToken().Kind == TokenKind::TK_EOF) {
    return nullptr;
  }

  std::string Error("Malformed top level expression.");
  Logger.error(Error);
  throw ParseException(std::move(Error));
}

void Parser::checkNextTokenKind(TokenKind Kind, const std::string &Error) {
  Cache.readNextToken();
  if (Cache.getCurrentToken().Kind != Kind) {
    throw ParseException(Error);
  }
}

bool Parser::checkNextCachedTokenKind(TokenKind Kind) {
  if (!Cache.readAndCacheNextToken()) {
    return false;
  }

  return Cache.getCurrentToken().Kind == Kind;
}

bool Parser::isFunctionSig() {
  Logger.debug("Checking for a function signature");

  // Return type should be identifier.
  if (!checkNextCachedTokenKind(TokenKind::TK_Identifier)) {
    Logger.debug("Failed to find return type: {}. Not a function signature.",
                 Cache.getCurrentToken().Value);
    return false;
  }

  // Func name should be identifier.
  if (!checkNextCachedTokenKind(TokenKind::TK_Identifier)) {
    Logger.debug("Failed to find function name: {}. Not a function signature.",
                 Cache.getCurrentToken().Value);
    return false;
  }

  if (!checkNextCachedTokenKind(TokenKind::TK_OpenParen)) {
    Logger.debug("Failed to find open paren to indicate argument list: {}. Not "
                 "a function signature.",
                 Cache.getCurrentToken().Value);
    return false;
  }

  // Check there's a close bracket after the args.
  while (Cache.readAndCacheNextToken() &&
         Cache.getCurrentToken().Kind != TokenKind::TK_CloseParen) {
  }

  if (Cache.getCurrentToken().Kind == TokenKind::TK_CloseParen) {
    Logger.info("Found a function signature.");
    return true;
  }

  Logger.debug("Failed to find close paren to indicate end of argument list: "
               "{}. Not a function signature.",
               Cache.getCurrentToken().Value);
  return false;
}

bool Parser::isFunctionDecl() {
  if (!isFunctionSig()) {
    Logger.debug(
        "Failed to find function signature. Not a function declaration.");
    return false;
  }

  // Expecting semicolon (as opposed to open brace).
  if (!checkNextCachedTokenKind(TokenKind::TK_Semicolon)) {
    Logger.debug("Failed to find semicolon after function signature. Not a "
                 "function declaration.");
    return false;
  }

  Logger.info("Found a function declaration.");
  return true;
}

bool Parser::isFunctionDef() {
  if (!isFunctionSig()) {
    Logger.debug(
        "Failed to find a function signature. Not a function definition.");
    return false;
  }

  // Expecting open brace (as opposed to semicolon).
  if (!checkNextCachedTokenKind(TokenKind::TK_OpenBrace)) {
    Logger.debug("Failed to find open brace after function signature. Not a "
                 "function definition.");
    return false;
  }

  Logger.info("Found a function definition.");
  return true;
}

std::unique_ptr<ast::FunctionDecl> Parser::parseFunctionSig() {
  // Parse return type.
  Cache.readNextToken();
  auto Return = stringToCTypeKind(Cache.getCurrentToken().Value);
  if (Return == ast::CTypeKind::CTK_None) {
    std::string Error(
        fmt::format("Unrecognised type {}.", Cache.getCurrentToken().Value));
    Logger.error(Error);
    throw ParseException(std::move(Error));
  }

  // Parse name.
  Cache.readNextToken();
  assert(Cache.getCurrentToken().Kind == TokenKind::TK_Identifier);
  auto Name = Cache.getCurrentToken().Value;

  // Parse args.
  std::vector<std::pair<std::string, ast::CTypeKind>> Args;

  checkNextTokenKind(TokenKind::TK_OpenParen,
                     "Was expecting beginning of argument list.");

  // End of args list.
  while (Cache.getCurrentToken().Kind != TokenKind::TK_CloseParen) {
    // Arg type.
    Cache.readNextToken();
    auto Type = stringToCTypeKind(Cache.getCurrentToken().Value);

    // Arg name.
    Cache.readNextToken();
    auto ArgName = Cache.getCurrentToken().Value;

    // Add arg.
    Args.emplace_back(std::move(ArgName), Type);

    // Either a close bracket or a comma.
    Cache.readNextToken();
  }

  return std::make_unique<ast::FunctionDecl>(std::move(Name), Return,
                                             std::move(Args));
}

ast::ASTPtr Parser::parseFunctionDecl() {
  auto Decl = parseFunctionSig();

  checkNextTokenKind(TokenKind::TK_Semicolon,
                     "Expected semicolon at end of function declaration.");

  return Decl;
}

ast::ASTPtr Parser::parseFunctionDef() {
  auto Decl = parseFunctionSig();

  checkNextTokenKind(TokenKind::TK_OpenBrace,
                     "Expected open brace after function signature.");

  Cache.readAndCacheNextToken();
  std::vector<ast::ASTPtr> Body;
  while (Cache.getCurrentToken().Kind != TokenKind::TK_CloseBrace) {
    Body.push_back(parseStatement());
    Cache.readAndCacheNextToken();
  }

  return std::make_unique<ast::FunctionDef>(std::move(Decl), std::move(Body));
}

ast::ASTPtr Parser::parseStatement() {
  if (isVarDecl()) {
    Logger.info("Parsing variable declaration.");
    return parseVarDecl();
  } else {
    // Placeholder to skip over statements.
    Logger.info("Skipping over statement.");
    while (Cache.getCurrentToken().Kind != TokenKind::TK_Semicolon) {
      Cache.readNextToken();
    }
  }

  return nullptr;
}

bool Parser::isVarDecl() {
  if (!Cache.readAndCacheNextToken()) {
    Logger.debug("No cached tokens. Not a variable declaration.");
    return false;
  }

  if (Cache.getCurrentToken().Kind == TokenKind::TK_Identifier &&
      stringToCTypeKind(Cache.getCurrentToken().Value) !=
          ast::CTypeKind::CTK_None) {
    Logger.info("Found a variable declaration.");
    return true;
  }

  Logger.debug("Leading token is not a type. Not a variable declaration.");
  return false;
}

ast::ASTPtr Parser::parseVarDecl() {
  // Get type.
  Cache.readNextToken();
  ast::CTypeKind Type = stringToCTypeKind(Cache.getCurrentToken().Value);

  // Get name.
  Cache.readNextToken();
  std::string Name = Cache.getCurrentToken().Value;

  auto Var = std::make_unique<ast::VariableDecl>(Type, std::move(Name));

  // Check whether its a semicolon or equals.
  Cache.readNextToken();
  if (Cache.getCurrentToken().Kind != TokenKind::TK_Equals &&
      Cache.getCurrentToken().Kind != TokenKind::TK_Semicolon) {
    std::string Error("Expected either end of statement or initialisation "
                      "after variable declaration");
    Logger.error(Error);
    throw ParseException(std::move(Error));
  }

  if (Cache.getCurrentToken().Kind == TokenKind::TK_Equals) {
    // Placeholder to skip over the assignment.
    while (Cache.getCurrentToken().Kind != TokenKind::TK_Semicolon) {
      Cache.readNextToken();
    }
  }

  return Var;
}

} // namespace fantac::parse
