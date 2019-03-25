#pragma once

#include "ASTInterfaces.h"

#include <cassert>

namespace fantac::ast {

class ASTLogger : public IASTVisitor {
public:
  ASTLogger();
  virtual ~ASTLogger() = default;

  // IASTVisitor impl.
  void visit(FunctionDecl &) override;
  void visit(FunctionDef &) override;
  void visit(VariableDecl &) override;
  void visit(UnaryOp &) override;
  void visit(BinaryOp &) override;
  void visit(IfCond &) override;
  void visit(TernaryCond &) override;
  void visit(IntegerLiteral &) override;
  void visit(FloatLiteral &) override;
  void visit(CharLiteral &) override;
  void visit(StringLiteral &) override;
  void visit(VariableRef &) override;
  void visit(WhileLoop &) override;
  void visit(ForLoop &) override;
  void visit(MemberAccess &) override;
  void visit(FunctionCall &) override;
  void visit(Return &) override;
};

} // namespace fantac::ast
