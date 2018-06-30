#include "FantaC.h"

#include <AST/ASTLogger.h>
#include <CodeGen/IRGenerator.h>
#include <Parse/TokenLexer.h>

#include <fstream>

namespace fantac {

void FantaC::run(const std::string &FileName) {
  // Setup the visitors we want.
  ASTVisitors.push_back(std::make_unique<ast::ASTLogger>());
  ASTVisitors.push_back(std::make_unique<codegen::IRGenerator>());

  std::ifstream File(FileName);

  Lexer.reset(new parse::TokenLexer(std::istreambuf_iterator<char>(File),
                                    std::istreambuf_iterator<char>()));

  try {
    // Lex source code into a stream of tokens.
    Lexer->lex();

    // Parse into AST.
    Parser.reset(new parse::Parser(Lexer->getTokens()));
    const auto &AST = Parser->parse();

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
