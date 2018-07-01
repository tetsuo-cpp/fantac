#include "Parser.h"

#include <algorithm>
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

template <typename TError, typename TCond, typename... TArgs>
void checkOrThrow(TError &&Error, TCond Cond, TArgs... Args) {
  if (!Cond(Args...)) {
    throw ParseException(std::forward<TError>(Error));
  }
}

} // anonymous namespace

Parser::Parser(const std::vector<Token> &Tokens)
    : Tokens(Tokens), TokenIter(Tokens.begin()) {
  static_cast<void>(this->Tokens);
}

const std::vector<ast::ASTPtr> &Parser::parse() {
  while (auto TopLevelNode = parseTopLevelExpr()) {
    AST.push_back(std::move(TopLevelNode));
  }

  return AST;
}

ast::ASTPtr Parser::parseTopLevelExpr() {
  auto *Tok = &*TokenIter;
  if (Tok->Kind == TokenKind::TK_EOF) {
    return nullptr;
  }

  // Find out what top level expr we're currently looking at and then parse it.
  if (isFunctionDecl()) {
    std::cout << "Parsing function decl.\n";
    return parseFunctionDecl();
  } else if (isFunctionDef()) {
    std::cout << "Parsing function def.\n";
    return parseFunctionDef();
  } else {
    throw ParseException("Parser: Malformed top level expression.");
  }
}

std::vector<Token>::const_iterator Parser::isFunctionSig() const {
  // Return type should be identifier.
  auto *Tok = &*TokenIter;
  if (Tok->Kind != TokenKind::TK_Identifier) {
    return Tokens.end();
  }

  // Func name should be identifier.
  Tok = &*(TokenIter + 1);
  if (Tok->Kind != TokenKind::TK_Identifier) {
    return Tokens.end();
  }

  Tok = &*(TokenIter + 2);
  if (Tok->Kind != TokenKind::TK_Symbol || Tok->Value != "(") {
    return Tokens.end();
  }

  // Check that there's a close bracket after the args.
  auto ArgEndIter =
      std::find_if(TokenIter + 2, Tokens.end(), [](const Token &Tok) {
        return Tok.Kind == TokenKind::TK_Symbol && Tok.Value == ")";
      });

  return ArgEndIter;
}

bool Parser::isFunctionDecl() const {
  auto FunctionSigIter = isFunctionSig();
  if (FunctionSigIter == Tokens.end()) {
    return false;
  }

  // Expecting semicolon (as opposed to open brace).
  auto *Tok = &*(FunctionSigIter + 1);
  if (Tok->Kind != TokenKind::TK_Symbol || Tok->Value != ";") {
    return false;
  }

  return true;
}

bool Parser::isFunctionDef() const {
  auto FunctionSigIter = isFunctionSig();
  if (FunctionSigIter == Tokens.end()) {
    return false;
  }

  // Expecting open brace (as opposed to semicolon).
  auto *Tok = &*(FunctionSigIter + 1);
  if (Tok->Kind != TokenKind::TK_Symbol || Tok->Value != "{") {
    return false;
  }

  return true;
}

std::unique_ptr<ast::FunctionDecl> Parser::parseFunctionSig() {
  auto *Tok = &*TokenIter++;

  // Parse return type.
  auto Return = stringToValueKind(Tok->Value);

  // Parse name.
  Tok = &*TokenIter++;
  assert(Tok->Kind == TokenKind::TK_Identifier);
  auto Name = Tok->Value;

  // Parse args.
  std::vector<std::pair<std::string, ast::ValueKind>> Args;

  Tok = &*TokenIter++;
  checkOrThrow("Parser: Expected open bracket in function definition.",
               [](const Token *Tok) {
                 return Tok && Tok->Kind == TokenKind::TK_Symbol &&
                        Tok->Value == "(";
               },
               Tok);

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

    // Either a close bracket or a comma.
    Tok = &*TokenIter++;
  }

  return std::make_unique<ast::FunctionDecl>(std::move(Name), Return,
                                             std::move(Args));
}

ast::ASTPtr Parser::parseFunctionDecl() {
  auto Decl = parseFunctionSig();

  auto *Tok = &*TokenIter++;
  checkOrThrow("Parser: Expected semicolon in function declaration.",
               [](const Token *Tok) {
                 return Tok && Tok->Kind == TokenKind::TK_Symbol &&
                        Tok->Value == ";";
               },
               Tok);

  return Decl;
}

ast::ASTPtr Parser::parseFunctionDef() {
  auto Decl = parseFunctionSig();

  auto *Tok = &*TokenIter++;
  checkOrThrow("Parser: Expected open brace in function definiton.",
               [](const Token *Tok) {
                 return Tok && Tok->Kind == TokenKind::TK_Symbol &&
                        Tok->Value == "{";
               },
               Tok);

  Tok = &*TokenIter++;
  std::vector<ast::ASTPtr> Body;
  while (Tok->Value != "}") {
    Body.push_back(parseStatement());
    Tok = &*++TokenIter;
  }

  ++TokenIter;

  return std::make_unique<ast::FunctionDef>(std::move(Decl), std::move(Body));
}

ast::ASTPtr Parser::parseStatement() {
  // Placeholder to skip over statements.
  TokenIter = std::find_if(TokenIter, Tokens.end(),
                           [](const Token &Tok) { return Tok.Value == ";"; });

  return nullptr;
}

} // namespace fantac::parse
