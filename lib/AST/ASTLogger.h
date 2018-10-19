#pragma once

#include "AST.h"

#include <Util/LoggerFactory.h>

#include <cassert>

namespace fantac::ast {

class ASTLogger : public IASTVisitor {
public:
  ASTLogger(util::LoggerFactory &LF);
  virtual ~ASTLogger() = default;

  // IASTVisitor impl.
  virtual void visit(FunctionDecl *AST) override;
  virtual void visit(FunctionDef *AST) override;
  virtual void visit(VariableDecl *AST) override;

private:
  spdlog::logger Logger;
};

ASTLogger::ASTLogger(util::LoggerFactory &LF)
    : Logger(LF.createLogger("ASTLogger")) {}

inline void ASTLogger::visit(FunctionDecl *AST) {
  assert(AST);
  Logger.info("FunctionDecl: {}", *AST);
}

inline void ASTLogger::visit(FunctionDef *AST) {
  assert(AST);
  Logger.info("FunctionDef: {}", *AST);
  for (const auto &Instruction : AST->Body) {
    if (Instruction) {
      Instruction->accept(this);
    }
  }
}

inline void ASTLogger::visit(VariableDecl *AST) {
  assert(AST);
  Logger.info("VariableDecl: {}", *AST);
}

} // namespace fantac::ast
