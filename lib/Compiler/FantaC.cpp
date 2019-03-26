#include "FantaC.h"

#include <CodeGen/IRGenerator.h>
#include <Parse/Lexer.h>
#include <Parse/Parser.h>

#include <fmt/format.h>

#include <fstream>

namespace fantac {

void run(const std::string &FileName) {
  std::ifstream File(FileName);
  std::string Source((std::istreambuf_iterator<char>(File)),
                     std::istreambuf_iterator<char>());

  // Construct parsing components.
  parse::Lexer L(&*Source.begin(), &*(Source.end() - 1));
  parse::Parser P(L);

  // Construct LLVM code generator.
  codegen::IRGenerator IR;

  try {
    // Parse into AST and generate LLVM IR.
    while (auto AST = P.parseTopLevelExpr()) {
#ifndef NDEBUG
      fmt::print("{};\n\n", AST->toString());
#endif
      AST->accept(IR);
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
