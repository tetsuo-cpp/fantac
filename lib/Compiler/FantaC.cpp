#include "FantaC.h"

#include <CodeGen/IRGenerator.h>
#include <Parse/Lexer.h>
#include <Parse/Parser.h>

#include <fmt/format.h>

#include <fstream>

namespace fantac {

FantaC::FantaC() = default;

void FantaC::run(const std::string &FileName) {
  // Setup the visitors we want.
  ASTVisitors.push_back(std::make_unique<codegen::IRGenerator>());

  std::ifstream File(FileName);
  std::string Source((std::istreambuf_iterator<char>(File)),
                     std::istreambuf_iterator<char>());

  try {
    // Construct parsing components.
    Lexer =
        std::make_unique<parse::Lexer>(&*Source.begin(), &*(Source.end() - 1));
    Parser = std::make_unique<parse::Parser>(*Lexer);

    // Parse into AST and generate LLVM IR.
    while (auto AST = Parser->parseTopLevelExpr()) {
#ifndef NDEBUG
      fmt::print("{};\n\n", AST->toString());
#endif
      for (auto &ASTVisitor : ASTVisitors) {
        AST->accept(*ASTVisitor);
      }
    }
  } catch (const parse::ParseException &Error) {
    fmt::print("Caught ParseException: \"{}\". Terminating compilation.",
               Error.what());
  } catch (const codegen::CodeGenException &Error) {
    fmt::print("Caught CodeGenException: \"{}\". Terminating compilation.",
               Error.what());
  }
}

} // namespace fantac
