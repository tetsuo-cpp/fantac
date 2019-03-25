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

  virtual void visit(FunctionDecl &) = 0;
  virtual void visit(FunctionDef &) = 0;
  virtual void visit(VariableDecl &) = 0;
  virtual void visit(UnaryOp &) = 0;
  virtual void visit(BinaryOp &) = 0;
  virtual void visit(IfCond &) = 0;
  virtual void visit(TernaryCond &) = 0;
  virtual void visit(IntegerLiteral &) = 0;
  virtual void visit(FloatLiteral &) = 0;
  virtual void visit(CharLiteral &) = 0;
  virtual void visit(StringLiteral &) = 0;
  virtual void visit(VariableRef &) = 0;
  virtual void visit(WhileLoop &) = 0;
  virtual void visit(ForLoop &) = 0;
  virtual void visit(MemberAccess &) = 0;
  virtual void visit(FunctionCall &) = 0;
  virtual void visit(Return &) = 0;
};

// Base class for all AST nodes.
struct IAST {
  virtual ~IAST() = default;

  virtual void accept(IASTVisitor &) = 0;

  llvm::Value *LLVMValue = nullptr;
};

using ASTPtr = std::unique_ptr<IAST>;

} // namespace fantac::ast
