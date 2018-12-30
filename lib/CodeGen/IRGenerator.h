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
  IRGenerator(util::ILoggerFactory &LF);
  virtual ~IRGenerator();

  // IASTVisitor impl.
  virtual void visit(ast::FunctionDecl &AST) override;
  virtual void visit(ast::FunctionDef &AST) override;
  virtual void visit(ast::VariableDecl &AST) override;
  virtual void visit(ast::UnaryOp &AST) override;
  virtual void visit(ast::BinaryOp &AST) override;
  virtual void visit(ast::IfCond &AST) override;
  virtual void visit(ast::TernaryCond &AST) override;
  virtual void visit(ast::IntegerLiteral &AST) override;
  virtual void visit(ast::FloatLiteral &AST) override;
  virtual void visit(ast::CharLiteral &AST) override;
  virtual void visit(ast::StringLiteral &AST) override;
  virtual void visit(ast::VariableRef &AST) override;
  virtual void visit(ast::WhileLoop &AST) override;
  virtual void visit(ast::ForLoop &AST) override;
  virtual void visit(ast::MemberAccess &AST) override;
  virtual void visit(ast::FunctionCall &AST) override;
  virtual void visit(ast::Return &AST) override;

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

  llvm::LLVMContext Context;
  llvm::IRBuilder<> Builder;
  llvm::Module Module;
  std::map<std::string, llvm::AllocaInst *> NamedVariables;
  std::map<std::string, llvm::Function *> Functions;
  std::unique_ptr<spdlog::logger> Logger;
};

} // namespace fantac::codegen
