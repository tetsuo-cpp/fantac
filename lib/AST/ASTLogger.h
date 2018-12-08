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
  virtual void visit(FunctionDecl &AST) override;
  virtual void visit(FunctionDef &AST) override;
  virtual void visit(VariableDecl &AST) override;
  virtual void visit(UnaryOp &AST) override;
  virtual void visit(BinaryOp &AST) override;
  virtual void visit(IfCond &AST) override;
  virtual void visit(TernaryCond &AST) override;
  virtual void visit(NumberLiteral &AST) override;
  virtual void visit(StringLiteral &AST) override;
  virtual void visit(VariableRef &AST) override;
  virtual void visit(WhileLoop &AST) override;
  virtual void visit(ForLoop &AST) override;
  virtual void visit(MemberAccess &AST) override;
  virtual void visit(FunctionCall &AST) override;

private:
  std::unique_ptr<spdlog::logger> Logger;
};

} // namespace fantac::ast
