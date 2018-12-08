#include "ASTLogger.h"

#include "AST.h"

namespace fantac::ast {

ASTLogger::ASTLogger(util::ILoggerFactory &LF)
    : Logger(LF.createLogger("ASTLogger")) {}

void ASTLogger::visit(FunctionDecl &AST) {
  Logger->info("FunctionDecl: {}.", AST);
}

void ASTLogger::visit(FunctionDef &AST) {
  Logger->info("FunctionDef: {}.", AST);

  for (const auto &Instruction : AST.Body) {
    if (Instruction) {
      Instruction->accept(*this);
    }
  }

  Logger->info("FunctionDef: End.");
}

void ASTLogger::visit(VariableDecl &AST) {
  Logger->info("VariableDecl: {}.", AST);
}

void ASTLogger::visit(UnaryOp &AST) {
  Logger->info("UnaryOp: {}.", AST);

  if (AST.Expr) {
    AST.Expr->accept(*this);
  }
}

void ASTLogger::visit(BinaryOp &AST) {
  Logger->info("BinaryOp: {}.", AST);

  if (AST.Left) {
    AST.Left->accept(*this);
  }

  if (AST.Right) {
    AST.Right->accept(*this);
  }
}

void ASTLogger::visit(IfCond &AST) {
  Logger->info("IfCond: {}.");

  if (AST.Condition) {
    AST.Condition->accept(*this);
  }

  Logger->info("IfCond: Then.");

  for (const auto &Instruction : AST.Then) {
    if (Instruction) {
      Instruction->accept(*this);
    }
  }

  Logger->info("IfCond: Else.");

  for (const auto &Instruction : AST.Else) {
    if (Instruction) {
      Instruction->accept(*this);
    }
  }

  Logger->info("IfCond: End.");
}

void ASTLogger::visit(TernaryCond &AST) {
  Logger->info("TernaryCond: {}.");

  if (AST.Condition) {
    AST.Condition->accept(*this);
  }

  Logger->info("TernaryCond: Then.");

  if (AST.Then) {
    AST.Then->accept(*this);
  }

  Logger->info("TernaryCond: Else.");

  if (AST.Else) {
    AST.Else->accept(*this);
  }

  Logger->info("TernaryCond: End.");
}

void ASTLogger::visit(NumberLiteral &AST) {
  Logger->info("NumberLiteral: {}.", AST);
}

void ASTLogger::visit(StringLiteral &AST) {
  Logger->info("StringLiteral: {}.", AST);
}

void ASTLogger::visit(VariableRef &AST) {
  Logger->info("VariableRef: {}.", AST);
}

void ASTLogger::visit(WhileLoop &AST) {
  Logger->info("WhileLoop: {}.");

  if (AST.Condition) {
    AST.Condition->accept(*this);
  }

  for (const auto &Instruction : AST.Body) {
    if (Instruction) {
      Instruction->accept(*this);
    }
  }

  Logger->info("WhileLoop: End.");
}

void ASTLogger::visit(ForLoop &AST) {
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

  Logger->info("ForLoop: End.");
}

} // namespace fantac::ast
