#include "Parser.h"

#include <cassert>
#include <iostream>

namespace fantac::parse {

namespace {

ast::ValueKind stringToValueKind(const std::string &Type) {
  if (Type == "int") {
    return ast::ValueKind::VK_Int;
  } else if (Type == "long") {
    return ast::ValueKind::VK_Long;
  } else if (Type == "float") {
    return ast::ValueKind::VK_Float;
  } else if (Type == "double") {
    return ast::ValueKind::VK_Double;
  } else {
    return ast::ValueKind::VK_Char;
  }
}

} // anonymous namespace

Parser::Parser(const std::vector<Token> &Tokens)
    : Tokens(Tokens), TokenIter(Tokens.begin()) {
  static_cast<void>(this->Tokens);
}

void Parser::parse() {
  while (auto TopLevelNode = parseTopLevelExpr()) {
    AST.push_back(std::move(TopLevelNode));
  }
}

ast::ASTPtr Parser::parseTopLevelExpr() {
  auto *Tok = &*TokenIter;
  if (Tok->Kind == TokenKind::TK_EOF) {
    return nullptr;
  }

  // Find out what top level expr we're currently looking at and then parse it.
  if (isFunctionDef()) {
    return parseFunctionDef();
  } else if (isFunctionDecl()) {
    return parseFunctionDecl();
  } else {
    throw ParseException("Parser: Malformed top level expression.");
  }
}

bool Parser::isFunctionDef() const { return true; }

bool Parser::isFunctionDecl() const { return false; }

ast::ASTPtr Parser::parseFunctionDef() {
  auto *Tok = &*TokenIter++;

  // Parse return type.
  auto Return = stringToValueKind(Tok->Value);

  // Parse name.
  Tok = &*TokenIter++;
  assert(Tok->Kind == TokenKind::TK_Identifier);
  auto Name = Tok->Value;

  // Parse args.
  std::vector<std::pair<std::string, ast::ValueKind>> Args;

  // Either an open bracket or a comma.
  Tok = &*TokenIter++;
  assert(Tok->Kind == TokenKind::TK_Symbol);
  assert(Tok->Value == "(");

  // End of args list.
  while (Tok->Value != ")") {
    // Arg type.
    Tok = &*TokenIter++;
    auto Type = stringToValueKind(Tok->Value);

    // Arg name.
    Tok = &*TokenIter++;
    auto ArgName = Tok->Value;

    // Add arg.
    Args.emplace_back(std::move(ArgName), Type);

    Tok = &*TokenIter++;
  }

  Tok = &*TokenIter++;
  assert(Tok->Kind == TokenKind::TK_Symbol);
  assert(Tok->Value == "{");

  Tok = &*TokenIter++;
  std::vector<ast::ASTPtr> Body;
  while (Tok->Value != "}") {
    Body.push_back(parseStatement());
    Tok = &*TokenIter++;
  }

  auto Function = std::make_unique<ast::FunctionNode>(
      std::move(Name), Return, std::move(Args), std::move(Body));

  std::cout << "Parsed AST: " << *Function << ".\n";

  return Function;
}

ast::ASTPtr Parser::parseFunctionDecl() { return nullptr; }

ast::ASTPtr Parser::parseStatement() {
  // Placeholder to skip over statements.
  const Token *Tok = nullptr;
  do {
    Tok = &*TokenIter++;
  } while (Tok->Value != ";");

  return nullptr;
}

} // namespace fantac::parse
