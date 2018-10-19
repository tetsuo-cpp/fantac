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
    : Lexer(Lexer), CacheIndex(0), Logger(LF.createLogger("Parser")) {}

ast::ASTPtr Parser::parseTopLevelExpr() {
  Logger.info("Parsing top level expr.");
  resetCache();
  clearCache();

  // Find out what top level expr we're currently looking at and then parse it.
  if (isFunctionDecl()) {
    resetCache();
    Logger.info("Parsing function decl.");
    return parseFunctionDecl();
  }

  resetCache();

  if (isFunctionDef()) {
    resetCache();
    Logger.info("Parsing function def.");
    return parseFunctionDef();
  }

  if (Tok.Kind == TokenKind::TK_EOF) {
    return nullptr;
  }

  std::string Error("Malformed top level expression.");
  Logger.error(Error);
  throw ParseException(std::move(Error));
}

void Parser::readNextToken() {
  if (!TokenCache.empty()) {
    Tok = TokenCache.front();
    TokenCache.pop_front();
    return;
  }

  if (!Lexer.lex(Tok)) {
    std::string Error("Unexpected end of token stream.");
    Logger.error(Error);
    throw ParseException(std::move(Error));
  }
}

bool Parser::readAndCacheNextToken() {
  if (CacheIndex < TokenCache.size()) {
    Tok = TokenCache.at(CacheIndex++);
    return true;
  } else {
    if (!Lexer.lex(Tok)) {
      return false;
    }

    TokenCache.push_back(Tok);
    ++CacheIndex;
    return true;
  }
}

void Parser::resetCache() { CacheIndex = 0; }

void Parser::clearCache() { TokenCache.clear(); }

void Parser::checkNextTokenKind(TokenKind Kind, const std::string &Error) {
  readNextToken();
  if (Tok.Kind != Kind) {
    throw ParseException(Error);
  }
}

bool Parser::checkNextCachedTokenKind(TokenKind Kind) {
  if (!readAndCacheNextToken()) {
    return false;
  }

  return Tok.Kind == Kind;
}

bool Parser::isFunctionSig() {
  // Return type should be identifier.
  if (!checkNextCachedTokenKind(TokenKind::TK_Identifier)) {
    return false;
  }

  // Func name should be identifier.
  if (!checkNextCachedTokenKind(TokenKind::TK_Identifier)) {
    return false;
  }

  if (!checkNextCachedTokenKind(TokenKind::TK_OpenParen)) {
    return false;
  }

  // Check there's a close bracket after the args.
  while (readAndCacheNextToken() && Tok.Kind != TokenKind::TK_CloseParen) {
  }

  if (Tok.Kind == TokenKind::TK_CloseParen) {
    return true;
  }

  return false;
}

bool Parser::isFunctionDecl() {
  if (!isFunctionSig()) {
    return false;
  }

  // Expecting semicolon (as opposed to open brace).
  if (!checkNextCachedTokenKind(TokenKind::TK_Semicolon)) {
    return false;
  }

  return true;
}

bool Parser::isFunctionDef() {
  if (!isFunctionSig()) {
    return false;
  }

  // Expecting open brace (as opposed to semicolon).
  if (!checkNextCachedTokenKind(TokenKind::TK_OpenBrace)) {
    return false;
  }

  return true;
}

std::unique_ptr<ast::FunctionDecl> Parser::parseFunctionSig() {
  // Parse return type.
  readNextToken();
  auto Return = stringToCTypeKind(Tok.Value);
  if (Return == ast::CTypeKind::CTK_None) {
    std::string Error(fmt::format("Unrecognised type {}.", Tok.Value));
    Logger.error(Error);
    throw ParseException(std::move(Error));
  }

  // Parse name.
  readNextToken();
  assert(Tok.Kind == TokenKind::TK_Identifier);
  auto Name = Tok.Value;

  // Parse args.
  std::vector<std::pair<std::string, ast::CTypeKind>> Args;

  checkNextTokenKind(TokenKind::TK_OpenParen,
                     "Was expecting beginning of argument list.");

  // End of args list.
  while (Tok.Kind != TokenKind::TK_CloseParen) {
    // Arg type.
    readNextToken();
    auto Type = stringToCTypeKind(Tok.Value);

    // Arg name.
    readNextToken();
    auto ArgName = Tok.Value;

    // Add arg.
    Args.emplace_back(std::move(ArgName), Type);

    // Either a close bracket or a comma.
    readNextToken();
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

  readAndCacheNextToken();
  std::vector<ast::ASTPtr> Body;
  while (Tok.Kind != TokenKind::TK_CloseBrace) {
    Body.push_back(parseStatement());
    readAndCacheNextToken();
  }

  return std::make_unique<ast::FunctionDef>(std::move(Decl), std::move(Body));
}

ast::ASTPtr Parser::parseStatement() {
  if (isVarDecl()) {
    Logger.info("Parsing variable decl.");
    return parseVarDecl();
  } else {
    // Placeholder to skip over statements.
    while (Tok.Kind != TokenKind::TK_Semicolon) {
      readNextToken();
    }
  }

  return nullptr;
}

bool Parser::isVarDecl() {
  if (!readAndCacheNextToken()) {
    return false;
  }

  return Tok.Kind == TokenKind::TK_Identifier &&
         stringToCTypeKind(Tok.Value) != ast::CTypeKind::CTK_None;
}

ast::ASTPtr Parser::parseVarDecl() {
  // Get type.
  readNextToken();
  ast::CTypeKind Type = stringToCTypeKind(Tok.Value);

  // Get name.
  readNextToken();
  std::string Name = Tok.Value;

  auto Var = std::make_unique<ast::VariableDecl>(Type, std::move(Name));

  // Check whether its a semicolon or equals.
  readNextToken();
  if (Tok.Kind != TokenKind::TK_Equals && Tok.Kind != TokenKind::TK_Semicolon) {
    std::string Error("Expected either end of statement or initialisation "
                      "after variable declaration");
    Logger.error(Error);
    throw ParseException(std::move(Error));
  }

  if (Tok.Kind == TokenKind::TK_Equals) {
    // Placeholder to skip over the assignment.
    while (Tok.Kind != TokenKind::TK_Semicolon) {
      readNextToken();
    }
  }

  return Var;
}

} // namespace fantac::parse
