#include "Parser.h"

#include <AST/AST.h>

#include <algorithm>
#include <cassert>

namespace fantac::parse {

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

  const auto Type = parseType();
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

ast::ASTPtr Parser::parseFunction(ast::CType Return, std::string &&Name) {
  Logger->debug("Parsing function.");

  // Parse arguments.
  std::vector<std::pair<std::string, ast::CType>> Args;
  while (!consumeToken(TokenKind::TK_CloseParen)) {
    // Skip commas between arguments.
    if (!Args.empty()) {
      expectToken(TokenKind::TK_Comma);
    }

    // Argument type.
    const auto ArgType = parseType();

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
  Logger->debug("Parsing statement.");

  if (consumeToken(TokenKind::TK_If)) {
    // Conditional.
    return parseIfCond();
  } else if (consumeToken(TokenKind::TK_For)) {
    // For loop.
    return parseForLoop();
  } else if (consumeToken(TokenKind::TK_While)) {
    // While loop.
    return parseWhileLoop();
  } else if (consumeToken(TokenKind::TK_Return)) {
    // Return statement.
    Logger->info("Found return.");
    if (consumeToken(TokenKind::TK_Semicolon)) {
      // Should be in a void function. Maybe check this?
      return std::make_unique<ast::Return>(nullptr);
    }

    auto ReturnExpr = parseExpr();
    expectToken(TokenKind::TK_Semicolon);
    return std::make_unique<ast::Return>(std::move(ReturnExpr));
  }

  // Variable declaration.
  const auto IsBeginningOfVarDecl =
      CurrentToken.Kind == TokenKind::TK_Unsigned ||
      CurrentToken.Kind == TokenKind::TK_Void ||
      CurrentToken.Kind == TokenKind::TK_Long ||
      CurrentToken.Kind == TokenKind::TK_Char ||
      CurrentToken.Kind == TokenKind::TK_Int ||
      CurrentToken.Kind == TokenKind::TK_Float ||
      CurrentToken.Kind == TokenKind::TK_Double;

  if (IsBeginningOfVarDecl) {
    const auto Type = parseType();
    return parseVariableDecl(Type);
  }

  // Expression statement.
  auto Expr = parseExpr();
  expectToken(TokenKind::TK_Semicolon);
  return Expr;
}

ast::ASTPtr Parser::parseVariableDecl(ast::CType Type) {
  Logger->debug("Parsing variable declaration.");

  auto Name = CurrentToken.Value;
  expectToken(TokenKind::TK_Identifier);

  // Parse assignment.
  if (consumeToken(TokenKind::TK_Assign)) {
    Logger->info("Found variable declaration with assignment.");
    auto AssignmentExpr = parseExpr();
    expectToken(TokenKind::TK_Semicolon);
    return std::make_unique<ast::VariableDecl>(Type, std::move(Name),
                                               std::move(AssignmentExpr));
  }

  Logger->info("Found variable declaration with no assignment.");
  expectToken(TokenKind::TK_Semicolon);
  return std::make_unique<ast::VariableDecl>(Type, std::move(Name));
}

ast::ASTPtr Parser::parseIfCond() {
  Logger->debug("Parsing if conditional.");

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

  Logger->info("Found if conditional.");
  return std::make_unique<ast::IfCond>(std::move(Cond), std::move(Then),
                                       std::move(Else));
}

ast::ASTPtr Parser::parseWhileLoop() {
  Logger->debug("Parsing while loop.");

  expectToken(TokenKind::TK_OpenParen);
  auto Cond = parseExpr();
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

  Logger->info("Found while loop.");
  return std::make_unique<ast::WhileLoop>(std::move(Cond), std::move(Body));
}

ast::ASTPtr Parser::parseForLoop() {
  Logger->debug("Parsing for loop.");

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

  Logger->info("Found for loop.");
  return std::make_unique<ast::ForLoop>(std::move(Init), std::move(Cond),
                                        std::move(Iter), std::move(Body));
}

ast::ASTPtr Parser::parseExpr() {
  Logger->debug("Parsing expression.");

  auto Left = parseAssignment();
  const auto Operator = CurrentToken.Kind;
  if (!consumeToken(TokenKind::TK_Comma)) {
    return Left;
  }

  Logger->info("Found comma.");
  return std::make_unique<ast::BinaryOp>(Operator, std::move(Left),
                                         parseExpr());
}

ast::ASTPtr Parser::parsePrimaryExpr() {
  Logger->debug("Parsing primary expression.");

  const auto Kind = CurrentToken.Kind;
  auto Identifier = CurrentToken.Value;
  Lexer.lex(CurrentToken);
  switch (Kind) {
  case TokenKind::TK_IntegerLiteral:
    Logger->info("Found integer literal.");
    return std::make_unique<ast::IntegerLiteral>(std::stoi(Identifier));
  case TokenKind::TK_FloatLiteral:
    Logger->info("Found float literal.");
    return std::make_unique<ast::FloatLiteral>(std::stof(Identifier));
  case TokenKind::TK_CharLiteral:
    Logger->info("Found char literal.");
    return std::make_unique<ast::CharLiteral>(Identifier.front());
  case TokenKind::TK_StringLiteral:
    Logger->info("Found string literal.");
    return std::make_unique<ast::StringLiteral>(std::move(Identifier));
  case TokenKind::TK_Identifier: {
    if (consumeToken(TokenKind::TK_OpenParen)) {
      Logger->info("Found function call.");
      return parseFunctionCall(std::move(Identifier));
    }

    Logger->info("Found variable reference.");
    return std::make_unique<ast::VariableRef>(std::move(Identifier));
  }
  default:
    throw ParseException("Unknown primary expression.");
  }
}

ast::ASTPtr Parser::parseAssignment() {
  Logger->debug("Parsing assignment expression.");

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
    Logger->info("Found assignment.");
    return std::make_unique<ast::BinaryOp>(Operator, std::move(Left),
                                           parseAssignment());
  }

  return Left;
}

ast::ASTPtr Parser::parseTernary() {
  Logger->debug("Parsing ternary expression.");

  auto Cond = parseLogicalOr();
  if (!consumeToken(TokenKind::TK_Question)) {
    return Cond;
  }

  auto Then = parseExpr();
  expectToken(TokenKind::TK_Colon);
  auto Else = parseTernary();

  Logger->info("Found ternary expression.");
  return std::make_unique<ast::TernaryCond>(std::move(Cond), std::move(Then),
                                            std::move(Else));
}

ast::ASTPtr Parser::parseLogicalOr() {
  Logger->debug("Parsing logical or expression.");

  auto Cond = parseLogicalAnd();
  const auto Operator = CurrentToken.Kind;
  while (consumeToken(TokenKind::TK_LogicalOr)) {
    Logger->info("Found logical or expression.");
    Cond = std::make_unique<ast::BinaryOp>(Operator, std::move(Cond),
                                           parseLogicalAnd());
  }

  return Cond;
}

ast::ASTPtr Parser::parseLogicalAnd() {
  Logger->debug("Parsing logical and expression.");

  auto Left = parseBitwiseOr();
  const auto Operator = CurrentToken.Kind;
  while (consumeToken(TokenKind::TK_And)) {
    Logger->info("Found logical and expression.");
    Left = std::make_unique<ast::BinaryOp>(Operator, std::move(Left),
                                           parseLogicalAnd());
  }

  return Left;
}

ast::ASTPtr Parser::parseBitwiseOr() {
  Logger->debug("Parsing bitwise or expression.");

  auto Left = parseBitwiseXor();
  const auto Operator = CurrentToken.Kind;
  while (consumeToken(TokenKind::TK_Or)) {
    Logger->info("Found bitwise or expression.");
    Left = std::make_unique<ast::BinaryOp>(Operator, std::move(Left),
                                           parseBitwiseXor());
  }

  return Left;
}

ast::ASTPtr Parser::parseBitwiseXor() {
  Logger->debug("Parsing bitwise xor expression.");

  auto Left = parseBitwiseAnd();
  const auto Operator = CurrentToken.Kind;
  while (consumeToken(TokenKind::TK_Xor)) {
    Logger->info("Found bitwise xor expression.");
    Left = std::make_unique<ast::BinaryOp>(Operator, std::move(Left),
                                           parseBitwiseAnd());
  }

  return Left;
}

ast::ASTPtr Parser::parseBitwiseAnd() {
  Logger->debug("Parsing bitwise and expression.");

  auto Left = parseEquality();
  const auto Operator = CurrentToken.Kind;
  while (consumeToken(TokenKind::TK_And)) {
    Logger->info("Found bitwise and expression.");
    Left = std::make_unique<ast::BinaryOp>(Operator, std::move(Left),
                                           parseEquality());
  }

  return Left;
}

ast::ASTPtr Parser::parseEquality() {
  Logger->debug("Parsing equality expression.");

  auto Left = parseRelational();
  while (true) {
    const auto Operator = CurrentToken.Kind;
    if (consumeToken(TokenKind::TK_Equals) ||
        consumeToken(TokenKind::TK_NotEquals)) {
      Logger->info("Found equality expression.");
      Left = std::make_unique<ast::BinaryOp>(Operator, std::move(Left),
                                             parseRelational());
    } else {
      return Left;
    }
  }
}

ast::ASTPtr Parser::parseRelational() {
  Logger->debug("Parsing relational expression.");

  auto Left = parseShift();
  while (true) {
    const auto Operator = CurrentToken.Kind;
    if (consumeToken(TokenKind::TK_LessThan) ||
        consumeToken(TokenKind::TK_GreaterThan) ||
        consumeToken(TokenKind::TK_LessThanEq) ||
        consumeToken(TokenKind::TK_GreaterThanEq)) {
      Logger->info("Found relational expression.");
      Left = std::make_unique<ast::BinaryOp>(Operator, std::move(Left),
                                             parseShift());
    } else {
      return Left;
    }
  }
}

ast::ASTPtr Parser::parseShift() {
  Logger->debug("Parsing shift expression.");

  auto Left = parseAddition();
  while (true) {
    const auto Operator = CurrentToken.Kind;
    if (consumeToken(TokenKind::TK_ShiftLeft) ||
        consumeToken(TokenKind::TK_ShiftRight)) {
      Logger->info("Found shift expression.");
      Left = std::make_unique<ast::BinaryOp>(Operator, std::move(Left),
                                             parseAddition());
    } else {
      return Left;
    }
  }
}

ast::ASTPtr Parser::parseAddition() {
  Logger->debug("Parsing addition expression.");

  auto Left = parseMultiplication();
  while (true) {
    const auto Operator = CurrentToken.Kind;
    if (consumeToken(TokenKind::TK_Add) ||
        consumeToken(TokenKind::TK_Subtract)) {
      Logger->info("Found addition expression.");
      Left = std::make_unique<ast::BinaryOp>(Operator, std::move(Left),
                                             parseMultiplication());
    } else {
      return Left;
    }
  }
}

ast::ASTPtr Parser::parseMultiplication() {
  Logger->debug("Parsing multiplication expression.");

  auto Left = parseUnary();
  while (true) {
    const auto Operator = CurrentToken.Kind;
    if (consumeToken(TokenKind::TK_Multiply) ||
        consumeToken(TokenKind::TK_Divide) ||
        consumeToken(TokenKind::TK_Modulus)) {
      Logger->info("Found multiplication expression.");
      Left = std::make_unique<ast::BinaryOp>(Operator, std::move(Left),
                                             parseUnary());
    } else {
      return Left;
    }
  }
}

ast::ASTPtr Parser::parseUnary() {
  Logger->debug("Parsing unary expression.");

  const auto Operator = CurrentToken.Kind;
  if (consumeToken(TokenKind::TK_Subtract)) {
    Logger->info("Found negative unary expression.");
    auto Zero = std::make_unique<ast::IntegerLiteral>(0);
    return std::make_unique<ast::BinaryOp>(Operator, std::move(Zero),
                                           parseUnary());
  }

  if (consumeToken(TokenKind::TK_Multiply) || consumeToken(TokenKind::TK_Add) ||
      consumeToken(TokenKind::TK_Not) || consumeToken(TokenKind::TK_SizeOf)) {
    Logger->info("Found unary expression.");
    return std::make_unique<ast::UnaryOp>(Operator, parseUnary());
  }

  if (consumeToken(TokenKind::TK_Increment) ||
      consumeToken(TokenKind::TK_Decrement)) {
    Logger->info("Found pre-increment/pre-decrement expression.");
    auto One = std::make_unique<ast::IntegerLiteral>(1);
    return std::make_unique<ast::BinaryOp>(Operator == TokenKind::TK_Increment
                                               ? TokenKind::TK_Add
                                               : TokenKind::TK_Subtract,
                                           parseUnary(), std::move(One));
  }

  return parsePostfix();
}

ast::ASTPtr Parser::parsePostfix() {
  Logger->debug("Parsing postfix expression.");

  auto Left = parsePrimaryExpr();
  while (true) {
    const auto Operator = CurrentToken.Kind;

    // Post increment and decrement.
    if (consumeToken(TokenKind::TK_Increment) ||
        consumeToken(TokenKind::TK_Decrement)) {
      Logger->info("Found post-increment/post-decrement expression.");
      Left = std::make_unique<ast::UnaryOp>(Operator, std::move(Left));
      continue;
    }

    // Member access.
    if (consumeToken(TokenKind::TK_Period)) {
      Logger->info("Found member access expression.");
      Left = std::make_unique<ast::MemberAccess>(std::move(Left),
                                                 CurrentToken.Value);
      expectToken(TokenKind::TK_Identifier);
      continue;
    }

    // Member access thru pointer.
    if (consumeToken(TokenKind::TK_Arrow)) {
      Logger->info("Found member access through pointer expression.");
      Left = std::make_unique<ast::UnaryOp>(TokenKind::TK_Multiply,
                                            std::move(Left));
      Left = std::make_unique<ast::MemberAccess>(std::move(Left),
                                                 CurrentToken.Value);
      expectToken(TokenKind::TK_Identifier);
      continue;
    }

    // Array access.
    if (consumeToken(TokenKind::TK_OpenSquareBracket)) {
      Logger->info("Found array access expression.");
      Left = std::make_unique<ast::BinaryOp>(TokenKind::TK_Add, std::move(Left),
                                             parseAssignment());
      Left = std::make_unique<ast::UnaryOp>(TokenKind::TK_Multiply,
                                            std::move(Left));
      expectToken(TokenKind::TK_CloseSquareBracket);
      continue;
    }

    break;
  }

  return Left;
}

ast::ASTPtr Parser::parseFunctionCall(std::string &&FunctionName) {
  Logger->debug("Parsing function call.");

  std::vector<ast::ASTPtr> Args;
  while (!consumeToken(TokenKind::TK_CloseParen)) {
    if (!Args.empty()) {
      expectToken(TokenKind::TK_Comma);
    }

    Args.push_back(parseAssignment());
  }

  Logger->info("Found function call.");
  return std::make_unique<ast::FunctionCall>(std::move(FunctionName),
                                             std::move(Args));
}

ast::CType Parser::parseType() {
  const bool Unsigned = consumeToken(TokenKind::TK_Unsigned);

  const ast::CLengthKind Length = [this]() {
    if (consumeToken(TokenKind::TK_Short)) {
      return ast::CLengthKind::CLK_Short;
    } else if (consumeToken(TokenKind::TK_Long)) {
      if (consumeToken(TokenKind::TK_Long)) {
        return ast::CLengthKind::CLK_LongLong;
      } else {
        return ast::CLengthKind::CLK_Long;
      }
    } else {
      return ast::CLengthKind::CLK_Default;
    }
  }();

  const ast::CTypeKind Type = [this]() {
    if (consumeToken(TokenKind::TK_Int)) {
      return ast::CTypeKind::CTK_Int;
    } else if (consumeToken(TokenKind::TK_Char)) {
      return ast::CTypeKind::CTK_Char;
    } else if (consumeToken(TokenKind::TK_Float)) {
      return ast::CTypeKind::CTK_Float;
    } else if (consumeToken(TokenKind::TK_Double)) {
      return ast::CTypeKind::CTK_Double;
    } else if (consumeToken(TokenKind::TK_Void)) {
      return ast::CTypeKind::CTK_Void;
    } else {
      throw ParseException(fmt::format("Unknown type name: {}. Expected one of "
                                       "(void, char, int, float, double).",
                                       CurrentToken.Value));
    }
  }();

  unsigned int Pointer = 0;
  while (consumeToken(TokenKind::TK_Multiply)) {
    ++Pointer;
  }

  return ast::CType(Type, Length, Unsigned, Pointer);
}

} // namespace fantac::parse
