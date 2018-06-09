#include "Parser.h"

#include <cassert>
#include <iostream>

namespace fantac::parse {

namespace {

ast::TypeKind stringToTypeKind(const std::string &Type) {
  if (Type == "int") {
    return ast::TypeKind::TK_Int;
  } else if (Type == "long") {
    return ast::TypeKind::TK_Long;
  } else if (Type == "float") {
    return ast::TypeKind::TK_Float;
  } else if (Type == "double") {
    return ast::TypeKind::TK_Double;
  } else {
    return ast::TypeKind::TK_Char;
  }
}

} // anonymous namespace

Parser::Parser(const std::vector<Token> &Tokens)
    : Tokens(Tokens), TokenIndex(0) {}

void Parser::parse() {
  // while (auto TopLevelNode = parseTopLevelExpr()) {
  //   AST.push_back(std::move(TopLevelNode));
  // }
  AST.push_back(parseTopLevelExpr());
}

ast::ASTPtr Parser::parseTopLevelExpr() {
  auto *Tok = &Tokens.at(TokenIndex++);
  if (Tok->Kind == TokenKind::TK_EOF) {
    return nullptr;
  }

  // TODO: Let's just assume it's a function definition right now to get a feel
  // for how this works.

  // Parse return type.
  auto Return = stringToTypeKind(Tok->Value);

  // Parse name.
  Tok = &Tokens.at(TokenIndex++);
  assert(Tok->Kind == TokenKind::TK_Identifier);
  auto Name = Tok->Value;

  // Parse args.
  std::vector<std::pair<std::string, ast::TypeKind>> Args;

  // Either an open bracket or a comma.
  Tok = &Tokens.at(TokenIndex++);
  assert(Tok->Kind == TokenKind::TK_Symbol);
  assert(Tok->Value == "(");

  // End of args list.
  while (Tok->Value != ")") {
    // Arg type.
    Tok = &Tokens.at(TokenIndex++);
    auto Type = stringToTypeKind(Tok->Value);

    // Arg name.
    Tok = &Tokens.at(TokenIndex++);
    auto ArgName = Tok->Value;

    // Add arg.
    Args.emplace_back(std::move(ArgName), Type);

    Tok = &Tokens.at(TokenIndex);
    if (Tok->Value == ",") {
      TokenIndex++;
    }
  }

  // TODO: Parse the body.
  std::vector<ast::ASTPtr> Body;

  auto Function = std::make_unique<ast::FunctionNode>(
      std::move(Name), Return, std::move(Args), std::move(Body));

  std::cout << "Parsed AST: " << *Function << ".\n";

  return Function;
}

} // namespace fantac::parse
