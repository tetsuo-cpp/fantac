#pragma once

#include <memory>

namespace fantac::ast {

// Fwd declare all AST nodes.
struct FunctionDecl;
struct FunctionDef;
struct VariableDecl;
struct UnaryOp;
struct BinaryOp;
struct IfCond;
struct TernaryCond;
struct NumberLiteral;
struct StringLiteral;
struct VariableRef;
struct WhileLoop;
struct ForLoop;
struct MemberAccess;

// Visitor interface for walking the AST.
class IASTVisitor {
public:
  virtual ~IASTVisitor() = default;

  virtual void visit(FunctionDecl &AST) = 0;
  virtual void visit(FunctionDef &AST) = 0;
  virtual void visit(VariableDecl &AST) = 0;
  virtual void visit(UnaryOp &AST) = 0;
  virtual void visit(BinaryOp &AST) = 0;
  virtual void visit(IfCond &AST) = 0;
  virtual void visit(TernaryCond &AST) = 0;
  virtual void visit(NumberLiteral &AST) = 0;
  virtual void visit(StringLiteral &AST) = 0;
  virtual void visit(VariableRef &AST) = 0;
  virtual void visit(WhileLoop &AST) = 0;
  virtual void visit(ForLoop &AST) = 0;
  virtual void visit(MemberAccess &AST) = 0;
};

// Base class for all AST nodes.
struct IAST {
  virtual ~IAST() = default;

  virtual void accept(IASTVisitor &Visitor) = 0;
};

using ASTPtr = std::unique_ptr<IAST>;

} // namespace fantac::ast
