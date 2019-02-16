#pragma once

#include "ASTInterfaces.h"

#include <Util/UtilInterfaces.h>

#include <cassert>

namespace fantac::ast {

class ASTLogger : public IASTVisitor {
public:
  explicit ASTLogger(util::ILoggerFactory &LF);
  virtual ~ASTLogger() = default;

  // IASTVisitor impl.
  void visit(FunctionDecl &AST) override;
  void visit(FunctionDef &AST) override;
  void visit(VariableDecl &AST) override;
  void visit(UnaryOp &AST) override;
  void visit(BinaryOp &AST) override;
  void visit(IfCond &AST) override;
  void visit(TernaryCond &AST) override;
  void visit(IntegerLiteral &AST) override;
  void visit(FloatLiteral &AST) override;
  void visit(CharLiteral &AST) override;
  void visit(StringLiteral &AST) override;
  void visit(VariableRef &AST) override;
  void visit(WhileLoop &AST) override;
  void visit(ForLoop &AST) override;
  void visit(MemberAccess &AST) override;
  void visit(FunctionCall &AST) override;
  void visit(Return &AST) override;

private:
  std::unique_ptr<spdlog::logger> Logger;
};

} // namespace fantac::ast
