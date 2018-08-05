#include "FantaC.h"

#include <AST/ASTLogger.h>
#include <CodeGen/IRGenerator.h>
#include <Parse/Lexer.h>

#include <fstream>

namespace fantac {

void FantaC::run(const std::string &FileName) {
  // Setup the visitors we want.
  ASTVisitors.push_back(std::make_unique<ast::ASTLogger>());
  ASTVisitors.push_back(std::make_unique<codegen::IRGenerator>());

  std::ifstream File(FileName);
  std::string Source((std::istreambuf_iterator<char>(File)),
                     std::istreambuf_iterator<char>());

  try {
    Lexer.reset(new parse::Lexer(&*Source.begin(), &*(Source.end() - 1)));

    std::vector<parse::Token> Tokens;
    parse::Token Token;
    while (Lexer->lex(Token)) {
      Tokens.push_back(Token);
    }

    Token.assign(parse::TokenKind::TK_EOF);
    Tokens.push_back(Token);

    Parser.reset(new parse::Parser(Tokens));

    // Parse into AST.
    std::vector<ast::ASTPtr> AST;
    while (auto Node = Parser->parseTopLevelExpr()) {
      AST.push_back(std::move(Node));
    }

    // Walk AST and generate LLVM IR.
    for (auto &ASTNode : AST) {
      for (auto &ASTVisitor : ASTVisitors) {
        ASTNode->accept(ASTVisitor.get());
      }
    }
  } catch (const parse::ParseException &Error) {
    std::cerr << Error.what() << "\n";
  }
}

} // namespace fantac
