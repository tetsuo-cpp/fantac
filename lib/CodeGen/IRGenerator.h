#pragma once

#include <AST/ASTInterfaces.h>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

namespace fantac::ast {

struct CType;

} // namespace fantac::ast

namespace fantac::codegen {

class CodeGenException : public std::runtime_error {
public:
  template <typename T>
  explicit CodeGenException(T &&Error)
      : std::runtime_error(std::forward<T>(Error)) {}
  virtual ~CodeGenException() = default;
};

class IRGenerator : public ast::IASTVisitor {
public:
  IRGenerator();
  virtual ~IRGenerator();

  // IASTVisitor impl.
  void visit(ast::FunctionDecl &) override;
  void visit(ast::FunctionDef &) override;
  void visit(ast::VariableDecl &) override;
  void visit(ast::UnaryOp &) override;
  void visit(ast::BinaryOp &) override;
  void visit(ast::IfCond &) override;
  void visit(ast::TernaryCond &) override;
  void visit(ast::IntegerLiteral &) override;
  void visit(ast::FloatLiteral &) override;
  void visit(ast::CharLiteral &) override;
  void visit(ast::StringLiteral &) override;
  void visit(ast::VariableRef &) override;
  void visit(ast::WhileLoop &) override;
  void visit(ast::ForLoop &) override;
  void visit(ast::MemberAccess &) override;
  void visit(ast::FunctionCall &) override;
  void visit(ast::Return &) override;

private:
  template <typename T> void visitAndAssign(T &);
  llvm::Value *visitImpl(ast::FunctionDecl &);
  llvm::Value *visitImpl(ast::FunctionDef &);
  llvm::Value *visitImpl(ast::VariableDecl &);
  llvm::Value *visitImpl(ast::UnaryOp &);
  llvm::Value *visitImpl(ast::BinaryOp &);
  llvm::Value *visitImpl(ast::IfCond &);
  llvm::Value *visitImpl(ast::TernaryCond &);
  llvm::Value *visitImpl(ast::IntegerLiteral &);
  llvm::Value *visitImpl(ast::FloatLiteral &);
  llvm::Value *visitImpl(ast::CharLiteral &);
  llvm::Value *visitImpl(ast::StringLiteral &);
  llvm::Value *visitImpl(ast::VariableRef &);
  llvm::Value *visitImpl(ast::WhileLoop &);
  llvm::Value *visitImpl(ast::ForLoop &);
  llvm::Value *visitImpl(ast::MemberAccess &);
  llvm::Value *visitImpl(ast::FunctionCall &);
  llvm::Value *visitImpl(ast::Return &);
  llvm::AllocaInst *createEntryBlockAlloca(llvm::Function *,
                                           const std::string &, llvm::Type *);
  llvm::Type *cTypeToLLVMType(ast::CType);
  llvm::Value *greaterThan(llvm::Value *, llvm::Value *);
  llvm::Value *equals(llvm::Value *, llvm::Value *);
  llvm::Value *assign(llvm::Value *, llvm::Value *);
  llvm::Value *add(llvm::Value *, llvm::Value *);

  llvm::LLVMContext Context;
  llvm::IRBuilder<> Builder;
  llvm::Module Module;
  std::map<std::string, llvm::AllocaInst *> NamedVariables;
  std::map<std::string, llvm::Function *> Functions;
};

} // namespace fantac::codegen
