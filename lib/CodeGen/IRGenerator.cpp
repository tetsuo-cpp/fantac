#include "IRGenerator.h"

#include <llvm/IR/Verifier.h>

namespace fantac::codegen {

IRGenerator::IRGenerator(util::LoggerFactory &LF)
    : Builder(Context), Module("FantaC", Context),
      Logger(LF.createLogger("IRGenerator")) {}

IRGenerator::~IRGenerator() { Module.print(llvm::errs(), nullptr); }

void IRGenerator::visit(ast::FunctionDecl &AST) {
  Logger->info("Generating IR for FunctionDecl.");

  std::vector<llvm::Type *> Args(AST.Args.size(),
                                 llvm::Type::getInt32Ty(Context));

  llvm::FunctionType *FT =
      llvm::FunctionType::get(llvm::Type::getInt32Ty(Context), Args, false);

  llvm::Function *F = llvm::Function::Create(
      FT, llvm::Function::ExternalLinkage, AST.Name, &Module);

  unsigned int Index = 0;
  for (auto &Arg : F->args()) {
    Arg.setName(AST.Args[Index++].first);
  }

  Functions.emplace(AST.Name, F);
}

void IRGenerator::visit(ast::FunctionDef &AST) {
  Logger->info("Generating IR for FunctionDef.");

  llvm::Function *F = nullptr;
  const auto &Name = AST.Decl->Name;

  auto Iter = Functions.find(Name);
  if (Iter == Functions.end()) {
    AST.Decl->accept(*this);
    Iter = Functions.find(Name);
  }

  F = Iter->second;

  llvm::BasicBlock *BB = llvm::BasicBlock::Create(Context, "entry", F);
  Builder.SetInsertPoint(BB);

  for (const auto &Instruction : AST.Body) {
    if (Instruction) {
      Instruction->accept(*this);
    }
  }

  Builder.CreateRet(llvm::ConstantInt::get(llvm::Type::getInt32Ty(Context), 0));

  llvm::verifyFunction(*F);
}

void IRGenerator::visit(ast::VariableDecl &AST) {
  Logger->info("Generating IR for VariableDecl.");

  auto *F = Builder.GetInsertBlock()->getParent();

  llvm::Type *VariableType = llvm::Type::getInt32Ty(Context);
  llvm::Value *InitialValue = llvm::ConstantInt::get(VariableType, 0);

  auto *Alloca = createEntryBlockAlloca(F, AST.Name, VariableType);
  Builder.CreateStore(InitialValue, Alloca);
}

llvm::AllocaInst *IRGenerator::createEntryBlockAlloca(
    llvm::Function *F, const std::string &VariableName, llvm::Type *Type) {
  llvm::IRBuilder<> B(&F->getEntryBlock(), F->getEntryBlock().begin());

  return B.CreateAlloca(Type, nullptr, VariableName);
}

void IRGenerator::visit(ast::BinaryOp &AST) { static_cast<void>(AST); }

void IRGenerator::visit(ast::IfCond &AST) { static_cast<void>(AST); }

void IRGenerator::visit(ast::NumberLiteral &AST) { static_cast<void>(AST); }

void IRGenerator::visit(ast::StringLiteral &AST) { static_cast<void>(AST); }

void IRGenerator::visit(ast::VariableRef &AST) { static_cast<void>(AST); }

void IRGenerator::visit(ast::WhileLoop &AST) { static_cast<void>(AST); }

void IRGenerator::visit(ast::ForLoop &AST) { static_cast<void>(AST); }

} // namespace fantac::codegen
