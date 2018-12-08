#include "Parser.h"

#include <AST/AST.h>

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

Parser::Parser(ILexer &Lexer, util::ILoggerFactory &LF)
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
  Logger->info("Parsing function.");

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
  Logger->info("Parsing variable declaration.");

  expectToken(TokenKind::TK_Identifier);
  auto Name = CurrentToken.Value;
  expectToken(TokenKind::TK_Identifier);

  // TODO: Parse assignments.
  while (!consumeToken(TokenKind::TK_Semicolon) && Lexer.lex(CurrentToken)) {
  }

  return std::make_unique<ast::VariableDecl>(Type, std::move(Name));
}

ast::ASTPtr Parser::parseIfCond() {
  Logger->info("Parsing if conditional.");

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
  Logger->info("Parsing while loop.");

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
  Logger->info("Parsing for loop.");

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
  Logger->info("Parsing expression.");

  auto Left = parseAssignment();
  const auto Operator = CurrentToken.Kind;
  if (!consumeToken(TokenKind::TK_Comma)) {
    return Left;
  }

  return std::make_unique<ast::BinaryOp>(Operator, std::move(Left),
                                         parseExpr());
}

ast::ASTPtr Parser::parsePrimaryExpr() {
  Logger->info("Parsing primary expression.");

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
  Logger->info("Parsing assignment expression.");

  auto Left = parseTernary();
  const auto Operator = CurrentToken.Kind;

  if (consumeToken(TokenKind::TK_Assign) ||
      consumeToken(TokenKind::TK_MultiplyEq) ||
      consumeToken(TokenKind::TK_DivideEq) ||
      consumeToken(TokenKind::TK_AddEq) ||
      consumeToken(TokenKind::TK_SubtractEq) ||
      consumeToken(TokenKind::TK_ModulusEq) ||
      consumeToken(TokenKind::TK_AndEq) || consumeToken(TokenKind::TK_OrEq) ||
      consumeToken(TokenKind::TK_XorEq) ||
      consumeToken(TokenKind::TK_ShiftLeftEq) ||
      consumeToken(TokenKind::TK_ShiftRightEq)) {
    return std::make_unique<ast::BinaryOp>(Operator, std::move(Left),
                                           parseAssignment());
  }

  return Left;
}

ast::ASTPtr Parser::parseTernary() {
  Logger->info("Parsing ternary expression.");

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
  Logger->info("Parsing logical or expression.");

  auto Cond = parseLogicalAnd();
  const auto Operator = CurrentToken.Kind;
  while (consumeToken(TokenKind::TK_LogicalOr)) {
    Cond = std::make_unique<ast::BinaryOp>(Operator, std::move(Cond),
                                           parseLogicalAnd());
  }

  return Cond;
}

ast::ASTPtr Parser::parseLogicalAnd() {
  Logger->info("Parsing logical and expression.");

  auto Left = parseBitwiseOr();
  const auto Operator = CurrentToken.Kind;
  while (consumeToken(TokenKind::TK_And)) {
    Left = std::make_unique<ast::BinaryOp>(Operator, std::move(Left),
                                           parseLogicalAnd());
  }

  return Left;
}

ast::ASTPtr Parser::parseBitwiseOr() {
  Logger->info("Parsing bitwise or expression.");

  auto Left = parseBitwiseXor();
  const auto Operator = CurrentToken.Kind;
  while (consumeToken(TokenKind::TK_Or)) {
    Left = std::make_unique<ast::BinaryOp>(Operator, std::move(Left),
                                           parseBitwiseXor());
  }

  return Left;
}

ast::ASTPtr Parser::parseBitwiseXor() {
  Logger->info("Parsing bitwise xor expression.");

  auto Left = parseBitwiseAnd();
  const auto Operator = CurrentToken.Kind;
  while (consumeToken(TokenKind::TK_Xor)) {
    Left = std::make_unique<ast::BinaryOp>(Operator, std::move(Left),
                                           parseBitwiseAnd());
  }

  return Left;
}

ast::ASTPtr Parser::parseBitwiseAnd() {
  Logger->info("Parsing bitwise and expression.");

  auto Left = parseEquality();
  const auto Operator = CurrentToken.Kind;
  while (consumeToken(TokenKind::TK_And)) {
    Left = std::make_unique<ast::BinaryOp>(Operator, std::move(Left),
                                           parseEquality());
  }

  return Left;
}

ast::ASTPtr Parser::parseEquality() {
  Logger->info("Parsing equality expression.");

  auto Left = parseRelational();
  while (true) {
    const auto Operator = CurrentToken.Kind;
    if (consumeToken(TokenKind::TK_Equals) ||
        consumeToken(TokenKind::TK_NotEquals)) {
      Left = std::make_unique<ast::BinaryOp>(Operator, std::move(Left),
                                             parseRelational());
    } else {
      return Left;
    }
  }
}

ast::ASTPtr Parser::parseRelational() {
  Logger->info("Parsing relational expression.");

  auto Left = parseShift();
  while (true) {
    const auto Operator = CurrentToken.Kind;
    if (consumeToken(TokenKind::TK_LessThan) ||
        consumeToken(TokenKind::TK_GreaterThan) ||
        consumeToken(TokenKind::TK_LessThanEq) ||
        consumeToken(TokenKind::TK_GreaterThanEq)) {
      Left = std::make_unique<ast::BinaryOp>(Operator, std::move(Left),
                                             parseShift());
    } else {
      return Left;
    }
  }
}

ast::ASTPtr Parser::parseShift() {
  Logger->info("Parsing shift expression.");

  auto Left = parseAddition();
  while (true) {
    const auto Operator = CurrentToken.Kind;
    if (consumeToken(TokenKind::TK_ShiftLeft) ||
        consumeToken(TokenKind::TK_ShiftRight)) {
      Left = std::make_unique<ast::BinaryOp>(Operator, std::move(Left),
                                             parseAddition());
    } else {
      return Left;
    }
  }
}

ast::ASTPtr Parser::parseAddition() {
  Logger->info("Parsing addition expression.");

  auto Left = parseMultiplication();
  while (true) {
    const auto Operator = CurrentToken.Kind;
    if (consumeToken(TokenKind::TK_Add) ||
        consumeToken(TokenKind::TK_Subtract)) {
      Left = std::make_unique<ast::BinaryOp>(Operator, std::move(Left),
                                             parseMultiplication());
    } else {
      return Left;
    }
  }
}

ast::ASTPtr Parser::parseMultiplication() {
  Logger->info("Parsing multiplication expression.");

  auto Left = parseUnary();
  while (true) {
    const auto Operator = CurrentToken.Kind;
    if (consumeToken(TokenKind::TK_Multiply) ||
        consumeToken(TokenKind::TK_Divide) ||
        consumeToken(TokenKind::TK_Modulus)) {
      Left = std::make_unique<ast::BinaryOp>(Operator, std::move(Left),
                                             parseUnary());
    } else {
      return Left;
    }
  }
}

ast::ASTPtr Parser::parseUnary() {
  Logger->info("Parsing unary expression.");

  const auto Operator = CurrentToken.Kind;
  if (consumeToken(TokenKind::TK_Subtract)) {
    auto Zero = std::make_unique<ast::NumberLiteral>(0);
    return std::make_unique<ast::BinaryOp>(Operator, std::move(Zero),
                                           parseUnary());
  }

  if (consumeToken(TokenKind::TK_Multiply) || consumeToken(TokenKind::TK_Add) ||
      consumeToken(TokenKind::TK_Not) || consumeToken(TokenKind::TK_SizeOf)) {
    return std::make_unique<ast::UnaryOp>(Operator, parseUnary());
  }

  if (consumeToken(TokenKind::TK_Increment) ||
      consumeToken(TokenKind::TK_Decrement)) {
    auto One = std::make_unique<ast::NumberLiteral>(1);
    return std::make_unique<ast::BinaryOp>(Operator == TokenKind::TK_Increment
                                               ? TokenKind::TK_Add
                                               : TokenKind::TK_Subtract,
                                           parseUnary(), std::move(One));
  }

  return parsePostfix();
}

ast::ASTPtr Parser::parsePostfix() {
  Logger->info("Parsing postfix expression.");

  auto Left = parsePrimaryExpr();
  while (true) {
    const auto Operator = CurrentToken.Kind;

    // Post increment and decrement.
    if (consumeToken(TokenKind::TK_Increment) ||
        consumeToken(TokenKind::TK_Decrement)) {
      Left = std::make_unique<ast::UnaryOp>(Operator, std::move(Left));
      continue;
    }

    // Member access.
    if (consumeToken(TokenKind::TK_Period)) {
      continue;
    }

    // Member access thru pointer.
    if (consumeToken(TokenKind::TK_Arrow)) {
      continue;
    }

    // Array access.
    if (consumeToken(TokenKind::TK_OpenSquareBracket)) {
      continue;
    }

    break;
  }

  return Left;
}

} // namespace fantac::parse
