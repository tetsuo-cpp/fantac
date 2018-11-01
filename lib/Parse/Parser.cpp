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

} // namespace

Parser::Parser(ILexer &Lexer, util::LoggerFactory &LF)
    : Lexer(Lexer), Logger(LF.createLogger("Parser")) {
  Lexer.lex(CurrentToken);
}

ast::ASTPtr Parser::parseTopLevelExpr() {
  Logger->info("Parsing top level expression.");
  if (CurrentToken.Kind == TokenKind::TK_EOF) {
    Logger->info("Encountered end of file. Stopping parsing.");
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
    Logger->info("Found function declaration.");
    return Decl;
  }

  expectToken(TokenKind::TK_OpenBrace);
  Logger->info("Found function definition.");

  // Otherwise, function definition.
  std::vector<ast::ASTPtr> Body;
  while (!consumeToken(TokenKind::TK_CloseBrace)) {
    Body.push_back(parseStatement());
  }

  return std::make_unique<ast::FunctionDef>(std::move(Decl), std::move(Body));
}

ast::ASTPtr Parser::parseStatement() {
  if (consumeToken(TokenKind::TK_If)) {
    // Conditional.
    return parseIfCond();
  } else if (consumeToken(TokenKind::TK_For)) {
    // For loop.
    return parseForLoop();
  } else if (consumeToken(TokenKind::TK_While)) {
    // While loop.
    return parseWhileLoop();
  }

  // Variable declaration.
  const auto Type = stringToCTypeKind(CurrentToken.Value);
  if (Type != ast::CTypeKind::CTK_None) {
    return parseVariableDecl(Type);
  }

  Logger->debug("Skipping over statement.");

  // Skip over statement.
  while (!consumeToken(TokenKind::TK_Semicolon) && Lexer.lex(CurrentToken)) {
  }

  return nullptr;
}

ast::ASTPtr Parser::parseVariableDecl(ast::CTypeKind Type) {
  expectToken(TokenKind::TK_Identifier);
  auto Name = CurrentToken.Value;
  expectToken(TokenKind::TK_Identifier);

  // TODO: Parse assignments.
  while (!consumeToken(TokenKind::TK_Semicolon) && Lexer.lex(CurrentToken)) {
  }

  Logger->info("Found variable declaration.");

  return std::make_unique<ast::VariableDecl>(Type, std::move(Name));
}

ast::ASTPtr Parser::parseIfCond() {
  expectToken(TokenKind::TK_OpenParen);
  auto Cond = parseExpr();
  expectToken(TokenKind::TK_CloseParen);

  std::vector<ast::ASTPtr> Then, Else;

  if (consumeToken(TokenKind::TK_OpenBrace)) {
    while (!consumeToken(TokenKind::TK_CloseBrace)) {
      Then.push_back(parseStatement());
    }

    if (consumeToken(TokenKind::TK_Else)) {
      expectToken(TokenKind::TK_OpenBrace);
      while (!consumeToken(TokenKind::TK_CloseBrace)) {
        Else.push_back(parseStatement());
      }
    }
  } else {
    // Braceless conditional.
    Then.push_back(parseStatement());
    if (consumeToken(TokenKind::TK_Else)) {
      Else.push_back(parseStatement());
    }
  }

  return std::make_unique<ast::IfCond>(std::move(Cond), std::move(Then),
                                       std::move(Else));
}

ast::ASTPtr Parser::parseWhileLoop() {
  expectToken(TokenKind::TK_OpenParen);
  auto Cond = parseExpr();
  expectToken(TokenKind::TK_CloseParen);

  std::vector<ast::ASTPtr> Body;

  if (consumeToken(TokenKind::TK_CloseBrace)) {
    while (!consumeToken(TokenKind::TK_CloseBrace)) {
      Body.push_back(parseStatement());
    }
  } else {
    // Braceless loop.
    Body.push_back(parseStatement());
  }

  return std::make_unique<ast::WhileLoop>(std::move(Cond), std::move(Body));
}

ast::ASTPtr Parser::parseForLoop() {
  expectToken(TokenKind::TK_OpenParen);
  auto Init = parseStatement();
  auto Cond = parseExpr();
  expectToken(TokenKind::TK_Semicolon);
  auto Iter = parseExpr();
  expectToken(TokenKind::TK_CloseParen);

  std::vector<ast::ASTPtr> Body;

  if (consumeToken(TokenKind::TK_OpenBrace)) {
    while (!consumeToken(TokenKind::TK_CloseBrace)) {
      Body.push_back(parseStatement());
    }
  } else {
    // Braceless loop.
    Body.push_back(parseStatement());
  }

  return std::make_unique<ast::ForLoop>(std::move(Init), std::move(Cond),
                                        std::move(Iter), std::move(Body));
}

ast::ASTPtr Parser::parseExpr() {
  // Lots more to do here. Just get it working for the moment.
  auto Left = parsePrimaryExpr();
  auto Operator = CurrentToken.Value;
  Lexer.lex(CurrentToken);
  auto Right = parsePrimaryExpr();

  return std::make_unique<ast::BinaryOp>(std::move(Operator), std::move(Left),
                                         std::move(Right));
}

ast::ASTPtr Parser::parsePrimaryExpr() {
  // TODO: Support floats, function calls and more.
  const auto Kind = CurrentToken.Kind;
  Lexer.lex(CurrentToken);
  switch (Kind) {
  case TokenKind::TK_NumberLiteral:
    return std::make_unique<ast::NumberLiteral>(std::stoi(CurrentToken.Value));
  case TokenKind::TK_StringLiteral:
    return std::make_unique<ast::StringLiteral>(CurrentToken.Value);
  case TokenKind::TK_Identifier:
    return std::make_unique<ast::VariableRef>(CurrentToken.Value);
  default:
    throw ParseException("Unknown primary expression.");
  }
}

ast::ASTPtr Parser::parseAssignment() {
  auto Left = parseTernary();
  auto Operator = CurrentToken.Value;

  if (consumeToken(TokenKind::TK_Equals) ||
      consumeToken(TokenKind::TK_MultiplyEq) ||
      consumeToken(TokenKind::TK_DivideEq) ||
      consumeToken(TokenKind::TK_AddEq) ||
      consumeToken(TokenKind::TK_SubtractEq) ||
      consumeToken(TokenKind::TK_ModulusEq) ||
      consumeToken(TokenKind::TK_AndEq) || consumeToken(TokenKind::TK_OrEq) ||
      consumeToken(TokenKind::TK_XorEq) ||
      consumeToken(TokenKind::TK_ShiftLeftEq) ||
      consumeToken(TokenKind::TK_ShiftRightEq)) {
    return std::make_unique<ast::BinaryOp>(std::move(Operator), std::move(Left),
                                           parseAssignment());
  }

  return Left;
}

ast::ASTPtr Parser::parseTernary() {
  auto Cond = parseLogicalOr();
  if (!consumeToken(TokenKind::TK_Question)) {
    return Cond;
  }

  auto Then = parseExpr();
  expectToken(TokenKind::TK_Colon);
  auto Else = parseTernary();

  return std::make_unique<ast::TernaryCond>(std::move(Cond), std::move(Then),
                                            std::move(Else));
}

ast::ASTPtr Parser::parseLogicalOr() {
  auto Cond = parseLogicalAnd();
  auto Operator = CurrentToken.Value;
  while (consumeToken(TokenKind::TK_LogicalOr)) {
    Cond = std::make_unique<ast::BinaryOp>(std::move(Operator), std::move(Cond),
                                           parseLogicalAnd());
  }

  return Cond;
}

ast::ASTPtr Parser::parseLogicalAnd() {
  auto Left = parseBitwiseOr();
  const auto T = CurrentToken;
  while (consumeToken(TokenKind::TK_And)) {
    Left = std::make_unique<ast::BinaryOp>(T.Value, std::move(Left),
                                           parseLogicalAnd());
  }

  return Left;
}

ast::ASTPtr Parser::parseBitwiseOr() {
  auto Left = parseBitwiseXor();
  auto Operator = CurrentToken.Value;
  while (consumeToken(TokenKind::TK_Or)) {
    Left = std::make_unique<ast::BinaryOp>(std::move(Operator), std::move(Left),
                                           parseBitwiseXor());
  }

  return Left;
}

ast::ASTPtr Parser::parseBitwiseXor() {
  auto Left = parseBitwiseAnd();
  auto Operator = CurrentToken.Value;
  while (consumeToken(TokenKind::TK_Xor)) {
    Left = std::make_unique<ast::BinaryOp>(std::move(Operator), std::move(Left),
                                           parseBitwiseXor());
  }

  return Left;
}

ast::ASTPtr Parser::parseBitwiseAnd() {
  auto Left = parseEquality();
  auto Operator = CurrentToken.Value;
  while (consumeToken(TokenKind::TK_And)) {
    Left = std::make_unique<ast::BinaryOp>(std::move(Operator), std::move(Left),
                                           parseEquality());
  }

  return Left;
}

ast::ASTPtr Parser::parseEquality() {
  auto Left = parseRelational();
  while (true) {
    auto Operator = CurrentToken.Value;
    if (consumeToken(TokenKind::TK_Equals) ||
        consumeToken(TokenKind::TK_NotEquals)) {
      Left = std::make_unique<ast::BinaryOp>(
          std::move(Operator), std::move(Left), parseRelational());
    } else {
      return Left;
    }
  }
}

ast::ASTPtr Parser::parseRelational() {
  auto Left = parseShift();
  while (true) {
    auto Operator = CurrentToken.Value;
    if (consumeToken(TokenKind::TK_LessThan) ||
        consumeToken(TokenKind::TK_GreaterThan) ||
        consumeToken(TokenKind::TK_LessThanEq) ||
        consumeToken(TokenKind::TK_GreaterThanEq)) {
      Left = std::make_unique<ast::BinaryOp>(std::move(Operator),
                                             std::move(Left), parseShift());
    } else {
      return Left;
    }
  }
}

ast::ASTPtr Parser::parseShift() {
  auto Left = parseAddition();
  while (true) {
    auto Operator = CurrentToken.Value;
    if (consumeToken(TokenKind::TK_ShiftLeft) ||
        consumeToken(TokenKind::TK_ShiftRight)) {
      Left = std::make_unique<ast::BinaryOp>(std::move(Operator),
                                             std::move(Left), parseAddition());
    } else {
      return Left;
    }
  }
}

ast::ASTPtr Parser::parseAddition() {
  auto Left = parseMultiplication();
  while (true) {
    auto Operator = CurrentToken.Value;
    if (consumeToken(TokenKind::TK_Add) ||
        consumeToken(TokenKind::TK_Subtract)) {
      Left = std::make_unique<ast::BinaryOp>(
          std::move(Operator), std::move(Left), parseMultiplication());
    } else {
      return Left;
    }
  }
}

ast::ASTPtr Parser::parseMultiplication() {
  auto Left = parseUnary();
  while (true) {
    auto Operator = CurrentToken.Value;
    if (consumeToken(TokenKind::TK_Multiply) ||
        consumeToken(TokenKind::TK_Divide) ||
        consumeToken(TokenKind::TK_Modulus)) {
      Left = std::make_unique<ast::BinaryOp>(std::move(Operator),
                                             std::move(Left), parseUnary());
    } else {
      return Left;
    }
  }
}

ast::ASTPtr Parser::parseUnary() {
  auto Operator = CurrentToken.Value;

  if (consumeToken(TokenKind::TK_Subtract)) {
    auto Zero = std::make_unique<ast::NumberLiteral>(0);
    return std::make_unique<ast::BinaryOp>(std::move(Operator), std::move(Zero),
                                           parseUnary());
  }

  return nullptr;
}

} // namespace fantac::parse
