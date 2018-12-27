#include "FantaC.h"

#include <AST/ASTLogger.h>
#include <CodeGen/IRGenerator.h>
#include <Parse/Lexer.h>
#include <Parse/Parser.h>
#include <Util/LoggerFactory.h>

#include <fstream>

namespace fantac {

FantaC::FantaC() = default;

void FantaC::run(const std::string &FileName, const std::string &LoggingLevel) {
  const util::LoggerConfig Config{util::stringToLoggingLevel(LoggingLevel)};
  LF = std::make_unique<util::LoggerFactory>(Config);

  // Construct logger.
  auto Logger = LF->createLogger("FantaC");

  // Setup the visitors we want.
  ASTVisitors.push_back(std::make_unique<ast::ASTLogger>(*LF));
  ASTVisitors.push_back(std::make_unique<codegen::IRGenerator>(*LF));

  std::ifstream File(FileName);
  std::string Source((std::istreambuf_iterator<char>(File)),
                     std::istreambuf_iterator<char>());

  try {
    // Construct parsing components.
    Lexer = std::make_unique<parse::Lexer>(&*Source.begin(),
                                           &*(Source.end() - 1), *LF);
    Parser = std::make_unique<parse::Parser>(*Lexer, *LF);

    Logger->info("Parsing to AST.");

    // Parse into AST and generate LLVM IR.
    while (auto AST = Parser->parseTopLevelExpr()) {
      for (auto &ASTVisitor : ASTVisitors) {
        AST->accept(*ASTVisitor);
      }
    }
  } catch (const parse::ParseException &Error) {
    Logger->error("Caught ParseException: \"{}\". Terminating compilation.",
                  Error.what());
  } catch (const codegen::CodeGenException &Error) {
    Logger->error("Caught CodeGenException: \"{}\". Terminating compilation.",
                  Error.what());
  }
}

} // namespace fantac
