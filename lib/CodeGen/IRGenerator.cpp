#include "IRGenerator.h"

#include <AST/AST.h>

#include <llvm/IR/Verifier.h>

namespace fantac::codegen {

IRGenerator::IRGenerator(util::ILoggerFactory &LF)
    : Builder(Context), Module("FantaC", Context),
      Logger(LF.createLogger("IRGenerator")) {}

IRGenerator::~IRGenerator() { Module.print(llvm::errs(), nullptr); }

void IRGenerator::visit(ast::FunctionDecl &AST) { visitAndAssign(AST); }

void IRGenerator::visit(ast::FunctionDef &AST) { visitAndAssign(AST); }

void IRGenerator::visit(ast::VariableDecl &AST) { visitAndAssign(AST); }

void IRGenerator::visit(ast::UnaryOp &AST) { visitAndAssign(AST); }

void IRGenerator::visit(ast::BinaryOp &AST) { visitAndAssign(AST); }

void IRGenerator::visit(ast::IfCond &AST) { visitAndAssign(AST); }

void IRGenerator::visit(ast::TernaryCond &AST) { visitAndAssign(AST); }

void IRGenerator::visit(ast::NumberLiteral &AST) { visitAndAssign(AST); }

void IRGenerator::visit(ast::StringLiteral &AST) { visitAndAssign(AST); }

void IRGenerator::visit(ast::VariableRef &AST) { visitAndAssign(AST); }

void IRGenerator::visit(ast::WhileLoop &AST) { visitAndAssign(AST); }

void IRGenerator::visit(ast::ForLoop &AST) { visitAndAssign(AST); }

void IRGenerator::visit(ast::MemberAccess &AST) { visitAndAssign(AST); }

void IRGenerator::visit(ast::FunctionCall &AST) { visitAndAssign(AST); }

void IRGenerator::visit(ast::Return &AST) { visitAndAssign(AST); }

template <typename T> void IRGenerator::visitAndAssign(T &AST) {
  AST.LLVMValue = visitImpl(AST);
}

llvm::Value *IRGenerator::visitImpl(ast::FunctionDecl &AST) {
  Logger->info("Generating IR for FunctionDecl.");

  std::vector<llvm::Type *> ArgTypes;
  for (const auto &Arg : AST.Args) {
    ArgTypes.push_back(cTypeToLLVMType(Arg.second));
  }

  llvm::Type *ReturnType = cTypeToLLVMType(AST.Return);
  llvm::FunctionType *FT = llvm::FunctionType::get(ReturnType, ArgTypes, false);

  llvm::Function *F = llvm::Function::Create(
      FT, llvm::Function::ExternalLinkage, AST.Name, &Module);

  unsigned int Index = 0;
  for (auto &Arg : F->args()) {
    Arg.setName(AST.Args[Index++].first);
  }

  Functions.emplace(AST.Name, F);
  return nullptr;
}

llvm::Value *IRGenerator::visitImpl(ast::FunctionDef &AST) {
  Logger->info("Generating IR for FunctionDef.");

  const auto &Name = AST.Decl->Name;

  auto Iter = Functions.find(Name);
  if (Iter == Functions.end()) {
    AST.Decl->accept(*this);
    Iter = Functions.find(Name);
  }

  llvm::Function *F = Iter->second;
  llvm::BasicBlock *BB = llvm::BasicBlock::Create(Context, "entry", F);
  Builder.SetInsertPoint(BB);

  NamedVariables.clear();
  for (auto &Arg : F->args()) {
    llvm::AllocaInst *Alloca =
        createEntryBlockAlloca(F, Arg.getName(), Arg.getType());

    Builder.CreateStore(&Arg, Alloca);
    NamedVariables.emplace(Arg.getName(), Alloca);
  }

  for (const auto &Instruction : AST.Body) {
    Instruction->accept(*this);
  }

  llvm::verifyFunction(*F);
  return nullptr;
}

llvm::Value *IRGenerator::visitImpl(ast::VariableDecl &AST) {
  Logger->info("Generating IR for VariableDecl.");

  auto *F = Builder.GetInsertBlock()->getParent();

  llvm::Type *VariableType = cTypeToLLVMType(AST.Type);
  llvm::Value *InitialValue = llvm::ConstantInt::get(VariableType, 0);

  if (AST.AssignmentExpr) {
    AST.AssignmentExpr->accept(*this);
    InitialValue = AST.AssignmentExpr->LLVMValue;
  }

  auto *Alloca = createEntryBlockAlloca(F, AST.Name, VariableType);
  Builder.CreateStore(InitialValue, Alloca);
  NamedVariables.emplace(AST.Name, Alloca);
  return nullptr;
}

llvm::Value *IRGenerator::visitImpl(ast::UnaryOp &AST) {
  static_cast<void>(AST);
  return nullptr;
}

llvm::Value *IRGenerator::visitImpl(ast::BinaryOp &AST) {
  // Evaluate from left to right.
  AST.Left->accept(*this);
  AST.Right->accept(*this);

  switch (AST.Operator) {
  case parse::TokenKind::TK_LogicalAnd:
    return nullptr;
  case parse::TokenKind::TK_GreaterThan:
    return Builder.CreateICmpSGT(AST.Left->LLVMValue, AST.Right->LLVMValue);
  case parse::TokenKind::TK_Equals:
    return Builder.CreateICmpEQ(AST.Left->LLVMValue, AST.Right->LLVMValue);
  case parse::TokenKind::TK_Assign:
    return Builder.CreateStore(AST.Right->LLVMValue, AST.Left->LLVMValue);
  default:
    throw CodeGenException(
        fmt::format("Invalid binary operator {}.", AST.Operator));
  }
}

llvm::Value *IRGenerator::visitImpl(ast::IfCond &AST) {
  AST.Condition->accept(*this);
  auto *CondV = AST.Condition->LLVMValue;
  if (!CondV) {
    throw CodeGenException("Condition in if statement evaluates to void.");
  }

  llvm::Function *CurrentF = Builder.GetInsertBlock()->getParent();
  llvm::BasicBlock *ThenBB =
      llvm::BasicBlock::Create(Context, "then", CurrentF);
  llvm::BasicBlock *ElseBB = llvm::BasicBlock::Create(Context, "else");
  llvm::BasicBlock *MergeBB = llvm::BasicBlock::Create(Context, "ifcont");

  Builder.CreateCondBr(CondV, ThenBB, ElseBB);
  Builder.SetInsertPoint(ThenBB);

  for (const auto &Instruction : AST.Then) {
    Instruction->accept(*this);
  }

  Builder.CreateBr(MergeBB);
  ThenBB = Builder.GetInsertBlock();
  CurrentF->getBasicBlockList().push_back(ElseBB);
  Builder.SetInsertPoint(ElseBB);

  for (const auto &Instruction : AST.Else) {
    Instruction->accept(*this);
  }

  Builder.CreateBr(MergeBB);
  ElseBB = Builder.GetInsertBlock();
  CurrentF->getBasicBlockList().push_back(MergeBB);
  Builder.SetInsertPoint(MergeBB);
  return nullptr;
}

llvm::Value *IRGenerator::visitImpl(ast::TernaryCond &AST) {
  static_cast<void>(AST);
  return nullptr;
}

llvm::Value *IRGenerator::visitImpl(ast::NumberLiteral &AST) {
  return llvm::ConstantInt::get(Context, llvm::APInt(32, AST.Value));
}

llvm::Value *IRGenerator::visitImpl(ast::StringLiteral &AST) {
  return Builder.CreateGlobalStringPtr(AST.Value);
}

llvm::Value *IRGenerator::visitImpl(ast::VariableRef &AST) {
  const auto VarIter = NamedVariables.find(AST.Name);
  if (VarIter == NamedVariables.end()) {
    throw CodeGenException(
        fmt::format("Reference to non-existent variable name: {}.", AST.Name));
  }

  return Builder.CreateLoad(VarIter->second, AST.Name);
}

llvm::Value *IRGenerator::visitImpl(ast::WhileLoop &AST) {
  static_cast<void>(AST);
  return nullptr;
}

llvm::Value *IRGenerator::visitImpl(ast::ForLoop &AST) {
  static_cast<void>(AST);
  return nullptr;
}

llvm::Value *IRGenerator::visitImpl(ast::MemberAccess &AST) {
  static_cast<void>(AST);
  return nullptr;
}

llvm::Value *IRGenerator::visitImpl(ast::FunctionCall &AST) {
  const auto FunctionIter = Functions.find(AST.Name);
  if (FunctionIter == Functions.end()) {
    throw CodeGenException(fmt::format(
        "Found function call to unknown function name: {}.", AST.Name));
  }

  llvm::Function *F = FunctionIter->second;
  if (F->arg_size() != AST.Args.size()) {
    throw CodeGenException(fmt::format(
        "Incorrect number of arguments passed. Expected {} but got {}.",
        F->arg_size(), AST.Args.size()));
  }

  std::vector<llvm::Value *> ArgsV;
  for (auto &Arg : AST.Args) {
    Arg->accept(*this);
    ArgsV.push_back(Arg->LLVMValue);
  }

  return Builder.CreateCall(F, ArgsV);
}

llvm::Value *IRGenerator::visitImpl(ast::Return &AST) {
  if (AST.Expr) {
    AST.Expr->accept(*this);
    Builder.CreateRet(AST.Expr->LLVMValue);
  } else {
    Builder.CreateRetVoid();
  }

  return nullptr;
}

llvm::AllocaInst *IRGenerator::createEntryBlockAlloca(
    llvm::Function *F, const std::string &VariableName, llvm::Type *Type) {
  llvm::IRBuilder<> B(&F->getEntryBlock(), F->getEntryBlock().begin());

  return B.CreateAlloca(Type, nullptr, VariableName);
}

llvm::Type *IRGenerator::cTypeToLLVMType(ast::CType X) {
  // TODO: Implement short.
  llvm::Type *const Type = [this, X]() -> llvm::Type * {
    switch (X.Type) {
    case ast::CTypeKind::CTK_Void:
      return Builder.getVoidTy();
    case ast::CTypeKind::CTK_Char:
      return Builder.getInt8Ty();
    case ast::CTypeKind::CTK_Double:
      return Builder.getDoubleTy();
    case ast::CTypeKind::CTK_Float:
      return Builder.getFloatTy();
    case ast::CTypeKind::CTK_Int:
      switch (X.Length) {
      case ast::CLengthKind::CLK_Default:
      case ast::CLengthKind::CLK_Long:
        return Builder.getInt32Ty();
      case ast::CLengthKind::CLK_LongLong:
        return Builder.getInt64Ty();
      default:
        break;
      }
    default:
      break;
    }

    throw CodeGenException("Bad CType to LLVM Type mapping.");
  }();

  return X.Pointer ? Type->getPointerTo() : Type;
}

} // namespace fantac::codegen
