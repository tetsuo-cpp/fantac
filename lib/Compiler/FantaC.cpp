#include "FantaC.h"

#include <AST/ASTLogger.h>
#include <CodeGen/IRGenerator.h>
#include <Parse/Lexer.h>

#include <fstream>

namespace fantac {

FantaC::FantaC() : LFConfig{spdlog::level::level_enum::debug}, LF(LFConfig) {}

void FantaC::run(const std::string &FileName) {
  // Construct logger.
  auto &Logger = LF.createLogger("FantaC");

  // Setup the visitors we want.
  ASTVisitors.push_back(std::make_unique<ast::ASTLogger>(LF));
  ASTVisitors.push_back(std::make_unique<codegen::IRGenerator>(LF));

  std::ifstream File(FileName);
  std::string Source((std::istreambuf_iterator<char>(File)),
                     std::istreambuf_iterator<char>());

  try {
    // Construct parsing components.
    Lexer.reset(new parse::Lexer(&*Source.begin(), &*(Source.end() - 1), LF));
    Parser.reset(new parse::Parser(*Lexer, LF));

    Logger.info("Parsing to AST.");

    // Parse into AST and generate LLVM IR.
    while (auto AST = Parser->parseTopLevelExpr()) {
      for (auto &ASTVisitor : ASTVisitors) {
        AST->accept(*ASTVisitor);
      }
    }
  } catch (const parse::ParseException &Error) {
    Logger.error("Caught ParseException: \"{}\". Terminating compilation.",
                 Error.what());
  }
}

} // namespace fantac
