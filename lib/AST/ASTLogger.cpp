#include "ASTLogger.h"

#include "AST.h"

#include <fmt/format.h>

namespace fantac::ast {

ASTLogger::ASTLogger() = default;

void ASTLogger::visit(FunctionDecl &AST) {
  static_cast<void>(AST);
  // fmt::print("FunctionDecl: {}.", AST);
}

void ASTLogger::visit(FunctionDef &AST) {
  static_cast<void>(AST);
  // fmt::print("FunctionDef: {}.", AST);

  for (const auto &Instruction : AST.Body) {
    if (Instruction) {
      Instruction->accept(*this);
    }
  }

  // fmt::print("FunctionDef: End.");
}

void ASTLogger::visit(VariableDecl &AST) {
  static_cast<void>(AST);
  // fmt::print("VariableDecl: {}.", AST);

  if (AST.AssignmentExpr) {
    AST.AssignmentExpr->accept(*this);
  } else {
    // fmt::print("VariableDecl: No assignment.");
  }
}

void ASTLogger::visit(UnaryOp &AST) {
  static_cast<void>(AST);
  // fmt::print("UnaryOp: {}.", AST);

  if (AST.Expr) {
    AST.Expr->accept(*this);
  }
}

void ASTLogger::visit(BinaryOp &AST) {
  static_cast<void>(AST);
  // fmt::print("BinaryOp: {}.", AST);

  if (AST.Left) {
    AST.Left->accept(*this);
  }

  if (AST.Right) {
    AST.Right->accept(*this);
  }
}

void ASTLogger::visit(IfCond &AST) {
  static_cast<void>(AST);
  // fmt::print("IfCond: {}.");

  if (AST.Condition) {
    AST.Condition->accept(*this);
  }

  // fmt::print("IfCond: Then.");

  for (const auto &Instruction : AST.Then) {
    if (Instruction) {
      Instruction->accept(*this);
    }
  }

  // fmt::print("IfCond: Else.");

  for (const auto &Instruction : AST.Else) {
    if (Instruction) {
      Instruction->accept(*this);
    }
  }

  // fmt::print("IfCond: End.");
}

void ASTLogger::visit(TernaryCond &AST) {
  static_cast<void>(AST);
  // fmt::print("TernaryCond: {}.");

  if (AST.Condition) {
    AST.Condition->accept(*this);
  }

  // fmt::print("TernaryCond: Then.");

  if (AST.Then) {
    AST.Then->accept(*this);
  }

  // fmt::print("TernaryCond: Else.");

  if (AST.Else) {
    AST.Else->accept(*this);
  }

  // fmt::print("TernaryCond: End.");
}

void ASTLogger::visit(IntegerLiteral &AST) {
  static_cast<void>(AST);
  // fmt::print("IntegerLiteral: {}.", AST);
}

void ASTLogger::visit(FloatLiteral &AST) {
  static_cast<void>(AST);
  // fmt::print("FloatLiteral: {}.", AST);
}

void ASTLogger::visit(CharLiteral &AST) {
  static_cast<void>(AST);
  // fmt::print("CharLiteral: {}.", AST);
}

void ASTLogger::visit(StringLiteral &AST) {
  static_cast<void>(AST);
  // fmt::print("StringLiteral: {}.", AST);
}

void ASTLogger::visit(VariableRef &AST) {
  static_cast<void>(AST);
  // fmt::print("VariableRef: {}.", AST);
}

void ASTLogger::visit(WhileLoop &AST) {
  static_cast<void>(AST);
  // fmt::print("WhileLoop: {}.");

  if (AST.Condition) {
    AST.Condition->accept(*this);
  }

  for (const auto &Instruction : AST.Body) {
    if (Instruction) {
      Instruction->accept(*this);
    }
  }

  // fmt::print("WhileLoop: End.");
}

void ASTLogger::visit(ForLoop &AST) {
  static_cast<void>(AST);
  // fmt::print("ForLoop: {}.");

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

  // fmt::print("ForLoop: End.");
}

void ASTLogger::visit(MemberAccess &AST) {
  static_cast<void>(AST);
  // fmt::print("MemberAccess: {}.", AST);

  if (AST.Expr) {
    AST.Expr->accept(*this);
  }
}

void ASTLogger::visit(FunctionCall &AST) {
  static_cast<void>(AST);
  // fmt::print("FunctionCall: {}.", AST);

  for (auto &Arg : AST.Args) {
    Arg->accept(*this);
  }

  // fmt::print("FunctionCall: End.");
}

void ASTLogger::visit(Return &AST) {
  static_cast<void>(AST);
  // fmt::print("Return: {}.");

  if (AST.Expr) {
    AST.Expr->accept(*this);
  }
}

} // namespace fantac::ast
