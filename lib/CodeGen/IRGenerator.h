#pragma once

#include <AST/AST.h>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

namespace fantac::codegen {

class IRGenerator : public ast::IASTVisitor {
public:
  IRGenerator();
  virtual ~IRGenerator() = default;

  // IASTVisitor impl.
  virtual void visit(ast::FunctionNode *AST) override;

private:
  llvm::LLVMContext Context;
  llvm::IRBuilder<> Builder;
  llvm::Module Module;
};

} // namespace fantac::codegen
