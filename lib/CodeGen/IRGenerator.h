#pragma once

#include <AST/ASTInterfaces.h>
#include <Util/UtilInterfaces.h>

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
  explicit IRGenerator(util::ILoggerFactory &LF);
  virtual ~IRGenerator();

  // IASTVisitor impl.
  void visit(ast::FunctionDecl &AST) override;
  void visit(ast::FunctionDef &AST) override;
  void visit(ast::VariableDecl &AST) override;
  void visit(ast::UnaryOp &AST) override;
  void visit(ast::BinaryOp &AST) override;
  void visit(ast::IfCond &AST) override;
  void visit(ast::TernaryCond &AST) override;
  void visit(ast::IntegerLiteral &AST) override;
  void visit(ast::FloatLiteral &AST) override;
  void visit(ast::CharLiteral &AST) override;
  void visit(ast::StringLiteral &AST) override;
  void visit(ast::VariableRef &AST) override;
  void visit(ast::WhileLoop &AST) override;
  void visit(ast::ForLoop &AST) override;
  void visit(ast::MemberAccess &AST) override;
  void visit(ast::FunctionCall &AST) override;
  void visit(ast::Return &AST) override;

private:
  template <typename T> void visitAndAssign(T &AST);
  llvm::Value *visitImpl(ast::FunctionDecl &AST);
  llvm::Value *visitImpl(ast::FunctionDef &AST);
  llvm::Value *visitImpl(ast::VariableDecl &AST);
  llvm::Value *visitImpl(ast::UnaryOp &AST);
  llvm::Value *visitImpl(ast::BinaryOp &AST);
  llvm::Value *visitImpl(ast::IfCond &AST);
  llvm::Value *visitImpl(ast::TernaryCond &AST);
  llvm::Value *visitImpl(ast::IntegerLiteral &AST);
  llvm::Value *visitImpl(ast::FloatLiteral &AST);
  llvm::Value *visitImpl(ast::CharLiteral &AST);
  llvm::Value *visitImpl(ast::StringLiteral &AST);
  llvm::Value *visitImpl(ast::VariableRef &AST);
  llvm::Value *visitImpl(ast::WhileLoop &AST);
  llvm::Value *visitImpl(ast::ForLoop &AST);
  llvm::Value *visitImpl(ast::MemberAccess &AST);
  llvm::Value *visitImpl(ast::FunctionCall &AST);
  llvm::Value *visitImpl(ast::Return &AST);
  llvm::AllocaInst *createEntryBlockAlloca(llvm::Function *F,
                                           const std::string &VariableName,
                                           llvm::Type *Type);
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
  std::unique_ptr<spdlog::logger> Logger;
};

} // namespace fantac::codegen
