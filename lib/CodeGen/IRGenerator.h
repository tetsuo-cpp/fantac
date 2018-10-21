#pragma once

#include <AST/AST.h>
#include <Util/LoggerFactory.h>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

namespace fantac::codegen {

class IRGenerator : public ast::IASTVisitor {
public:
  IRGenerator(util::LoggerFactory &LF);
  virtual ~IRGenerator();

  // IASTVisitor impl.
  virtual void visit(ast::FunctionDecl &AST) override;
  virtual void visit(ast::FunctionDef &AST) override;
  virtual void visit(ast::VariableDecl &AST) override;

private:
  llvm::AllocaInst *createEntryBlockAlloca(llvm::Function *F,
                                           const std::string &VariableName,
                                           llvm::Type *Type);

  llvm::LLVMContext Context;
  llvm::IRBuilder<> Builder;
  llvm::Module Module;
  std::map<std::string, llvm::Function *> Functions;
  spdlog::logger &Logger;
};

} // namespace fantac::codegen
