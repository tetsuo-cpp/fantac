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
  } else if (Type == "char") {
    return ast::ValueKind::VK_Char;
  } else {
    return ast::ValueKind::VK_None;
  }
}

template <typename TError>
void enforceToken(TError &&Error, const Token *Token, TokenKind ExpectedKind,
                  const std::string &ExpectedValue) {
  if (Token && Token->Kind == ExpectedKind && Token->Value == ExpectedValue) {
    return;
  }

  throw ParseException(std::forward<TError>(Error));
}

} // anonymous namespace

Parser::Parser(const std::vector<Token> &Tokens)
    : Tokens(Tokens), TokenIter(Tokens.begin()) {}

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
  enforceToken("Parser: Was expecting beginning of argument list.", Tok,
               TokenKind::TK_Symbol, "(");

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
  enforceToken("Parser: Expected semicolon at end of function declaration.",
               Tok, TokenKind::TK_Symbol, ";");

  return Decl;
}

ast::ASTPtr Parser::parseFunctionDef() {
  auto Decl = parseFunctionSig();

  auto *Tok = &*TokenIter++;
  enforceToken("Parser: Expected open brace after function signature.", Tok,
               TokenKind::TK_Symbol, "{");

  Tok = &*TokenIter;
  std::vector<ast::ASTPtr> Body;
  while (Tok->Value != "}") {
    Body.push_back(parseStatement());
    Tok = &*++TokenIter;
  }

  ++TokenIter;

  return std::make_unique<ast::FunctionDef>(std::move(Decl), std::move(Body));
}

ast::ASTPtr Parser::parseStatement() {
  if (isVarDecl()) {
    std::cout << "Parsing variable decl.\n";
    return parseVarDecl();
  } else {
    // Placeholder to skip over statements.
    TokenIter = std::find_if(TokenIter, Tokens.end(),
                             [](const Token &Tok) { return Tok.Value == ";"; });
  }

  return nullptr;
}

bool Parser::isVarDecl() const {
  const auto &Tok = *TokenIter;
  return Tok.Kind == TokenKind::TK_Identifier &&
         stringToValueKind(Tok.Value) != ast::ValueKind::VK_None;
}

ast::ASTPtr Parser::parseVarDecl() {
  // Get type.
  auto *Tok = &*TokenIter++;
  ast::ValueKind Type = stringToValueKind(Tok->Value);

  // Get name.
  Tok = &*TokenIter++;
  std::string Name = Tok->Value;

  auto Var = std::make_unique<ast::VariableDecl>(Type, std::move(Name));

  // Check whether its a semicolon or equals.
  Tok = &*TokenIter++;

  if (Tok->Value != "=" && Tok->Value != ";") {
    throw ParseException("Parser: Expected either end of statement or "
                         "initialisation after variable declaration.");
  }

  if (Tok->Value == "=") {
    TokenIter = std::find_if(TokenIter, Tokens.end(),
                             [](const Token &Tok) { return Tok.Value == ";"; });
  }

  return Var;
}

} // namespace fantac::parse
