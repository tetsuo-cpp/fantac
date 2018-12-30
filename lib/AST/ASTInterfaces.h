#pragma once

#include <memory>

namespace llvm {

class Value;

} // namespace llvm

namespace fantac::ast {

// Fwd declare all AST nodes.
struct FunctionDecl;
struct FunctionDef;
struct VariableDecl;
struct UnaryOp;
struct BinaryOp;
struct IfCond;
struct TernaryCond;
struct IntegerLiteral;
struct FloatLiteral;
struct CharLiteral;
struct StringLiteral;
struct VariableRef;
struct WhileLoop;
struct ForLoop;
struct MemberAccess;
struct FunctionCall;
struct Return;

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
  virtual void visit(IntegerLiteral &AST) = 0;
  virtual void visit(FloatLiteral &AST) = 0;
  virtual void visit(CharLiteral &AST) = 0;
  virtual void visit(StringLiteral &AST) = 0;
  virtual void visit(VariableRef &AST) = 0;
  virtual void visit(WhileLoop &AST) = 0;
  virtual void visit(ForLoop &AST) = 0;
  virtual void visit(MemberAccess &AST) = 0;
  virtual void visit(FunctionCall &AST) = 0;
  virtual void visit(Return &AST) = 0;
};

// Base class for all AST nodes.
struct IAST {
  virtual ~IAST() = default;

  virtual void accept(IASTVisitor &Visitor) = 0;

  llvm::Value *LLVMValue = nullptr;
};

using ASTPtr = std::unique_ptr<IAST>;

} // namespace fantac::ast
