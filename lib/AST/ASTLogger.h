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
  virtual void visit(FunctionDecl &AST) override;
  virtual void visit(FunctionDef &AST) override;
  virtual void visit(VariableDecl &AST) override;
  virtual void visit(BinaryOp &AST) override;
  virtual void visit(IfCond &AST) override;
  virtual void visit(NumberLiteral &AST) override;
  virtual void visit(StringLiteral &AST) override;
  virtual void visit(VariableRef &AST) override;
  virtual void visit(WhileLoop &AST) override;
  virtual void visit(ForLoop &AST) override;

private:
  std::unique_ptr<spdlog::logger> Logger;
};

ASTLogger::ASTLogger(util::LoggerFactory &LF)
    : Logger(LF.createLogger("ASTLogger")) {}

inline void ASTLogger::visit(FunctionDecl &AST) {
  Logger->info("FunctionDecl: {}.", AST);
}

inline void ASTLogger::visit(FunctionDef &AST) {
  Logger->info("FunctionDef: {}.", AST);
  for (const auto &Instruction : AST.Body) {
    if (Instruction) {
      Instruction->accept(*this);
    }
  }
}

inline void ASTLogger::visit(VariableDecl &AST) {
  Logger->info("VariableDecl: {}.", AST);
}

inline void ASTLogger::visit(BinaryOp &AST) {
  Logger->info("BinaryOp: {}.", AST);
  if (AST.Left) {
    AST.Left->accept(*this);
  }

  if (AST.Right) {
    AST.Right->accept(*this);
  }
}

inline void ASTLogger::visit(IfCond &AST) {
  Logger->info("IfCond: {}.");
  for (const auto &Instruction : AST.Then) {
    if (Instruction) {
      Instruction->accept(*this);
    }
  }

  for (const auto &Instruction : AST.Else) {
    if (Instruction) {
      Instruction->accept(*this);
    }
  }
}

inline void ASTLogger::visit(NumberLiteral &AST) {
  Logger->info("NumberLiteral: {}.", AST);
}

inline void ASTLogger::visit(StringLiteral &AST) {
  Logger->info("StringLiteral: {}.", AST);
}

inline void ASTLogger::visit(VariableRef &AST) {
  Logger->info("VariableRef: {}.", AST);
}

inline void ASTLogger::visit(WhileLoop &AST) {
  Logger->info("WhileLoop: {}.");
  if (AST.Condition) {
    AST.Condition->accept(*this);
  }

  for (auto &Instruction : AST.Body) {
    if (Instruction) {
      Instruction->accept(*this);
    }
  }
}

inline void ASTLogger::visit(ForLoop &AST) {
  Logger->info("ForLoop: {}.");
  if (AST.Init) {
    AST.Init->accept(*this);
  }

  if (AST.Condition) {
    AST.Condition->accept(*this);
  }

  if (AST.Iteration) {
    AST.Iteration->accept(*this);
  }

  for (auto &Instruction : AST.Body) {
    Instruction->accept(*this);
  }
}

} // namespace fantac::ast
