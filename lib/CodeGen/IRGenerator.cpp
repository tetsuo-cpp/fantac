#include "IRGenerator.h"

#include <AST/AST.h>

#include <fmt/format.h>
#include <llvm/IR/Verifier.h>

namespace fantac::codegen {

IRGenerator::IRGenerator() : Builder(Context), Module("FantaC", Context) {}

IRGenerator::~IRGenerator() { Module.print(llvm::errs(), nullptr); }

void IRGenerator::visit(ast::FunctionDecl &AST) { visitAndAssign(AST); }

void IRGenerator::visit(ast::FunctionDef &AST) { visitAndAssign(AST); }

void IRGenerator::visit(ast::VariableDecl &AST) { visitAndAssign(AST); }

void IRGenerator::visit(ast::UnaryOp &AST) { visitAndAssign(AST); }

void IRGenerator::visit(ast::BinaryOp &AST) { visitAndAssign(AST); }

void IRGenerator::visit(ast::IfCond &AST) { visitAndAssign(AST); }

void IRGenerator::visit(ast::TernaryCond &AST) { visitAndAssign(AST); }

void IRGenerator::visit(ast::IntegerLiteral &AST) { visitAndAssign(AST); }

void IRGenerator::visit(ast::FloatLiteral &AST) { visitAndAssign(AST); }

void IRGenerator::visit(ast::CharLiteral &AST) { visitAndAssign(AST); }

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
  auto *F = Builder.GetInsertBlock()->getParent();

  llvm::Type *VariableType = cTypeToLLVMType(AST.Type);
  llvm::Value *InitialValue = [this, &AST, VariableType]() -> llvm::Value * {
    if (AST.AssignmentExpr) {
      AST.AssignmentExpr->accept(*this);
      return AST.AssignmentExpr->LLVMValue;
    } else {
      return llvm::ConstantInt::get(VariableType, 0);
    }
  }();

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
    return greaterThan(AST.Left->LLVMValue, AST.Right->LLVMValue);
  case parse::TokenKind::TK_LessThan:
    return greaterThan(AST.Right->LLVMValue, AST.Left->LLVMValue);
  case parse::TokenKind::TK_Equals:
    return equals(AST.Left->LLVMValue, AST.Right->LLVMValue);
  case parse::TokenKind::TK_Assign:
    return assign(AST.Left->LLVMValue, AST.Right->LLVMValue);
  case parse::TokenKind::TK_Add:
    return add(AST.Left->LLVMValue, AST.Right->LLVMValue);
  default:
    throw CodeGenException(
        fmt::format("Invalid binary operator.", AST.Operator));
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
  AST.Condition->accept(*this);
  auto *CondV = AST.Condition->LLVMValue;
  if (!CondV) {
    throw CodeGenException("Condition in ternary statement evaluates to void.");
  }

  llvm::Function *CurrentF = Builder.GetInsertBlock()->getParent();
  llvm::BasicBlock *ThenBB =
      llvm::BasicBlock::Create(Context, "then", CurrentF);
  llvm::BasicBlock *ElseBB = llvm::BasicBlock::Create(Context, "else");
  llvm::BasicBlock *MergeBB = llvm::BasicBlock::Create(Context, "ifcont");

  Builder.CreateCondBr(CondV, ThenBB, ElseBB);
  Builder.SetInsertPoint(ThenBB);

  AST.Then->accept(*this);

  Builder.CreateBr(MergeBB);
  ThenBB = Builder.GetInsertBlock();
  CurrentF->getBasicBlockList().push_back(ElseBB);
  Builder.SetInsertPoint(ElseBB);

  AST.Else->accept(*this);

  Builder.CreateBr(MergeBB);
  ElseBB = Builder.GetInsertBlock();
  CurrentF->getBasicBlockList().push_back(MergeBB);
  Builder.SetInsertPoint(MergeBB);

  llvm::PHINode *PN =
      Builder.CreatePHI(AST.Then->LLVMValue->getType(), 2, "terntmp");
  PN->addIncoming(AST.Then->LLVMValue, ThenBB);
  PN->addIncoming(AST.Else->LLVMValue, ElseBB);
  return PN;
}

llvm::Value *IRGenerator::visitImpl(ast::IntegerLiteral &AST) {
  return llvm::ConstantInt::get(Context, llvm::APInt(32, AST.Value));
}

llvm::Value *IRGenerator::visitImpl(ast::FloatLiteral &AST) {
  return llvm::ConstantFP::get(Builder.getFloatTy(), AST.Value);
}

llvm::Value *IRGenerator::visitImpl(ast::CharLiteral &AST) {
  return llvm::ConstantInt::get(Builder.getInt8Ty(), AST.Value);
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
  llvm::Function *F = Builder.GetInsertBlock()->getParent();

  AST.Condition->accept(*this);
  llvm::Value *CondV = AST.Condition->LLVMValue;
  if (!CondV) {
    throw CodeGenException("Condition in while loop evaluates to void.");
  }

  llvm::BasicBlock *LoopBB = llvm::BasicBlock::Create(Context, "loop", F);
  llvm::BasicBlock *AfterLoopBB =
      llvm::BasicBlock::Create(Context, "afterloop", F);

  Builder.CreateCondBr(CondV, LoopBB, AfterLoopBB);
  F->getBasicBlockList().push_back(LoopBB);
  Builder.SetInsertPoint(LoopBB);

  for (const auto &Instruction : AST.Body) {
    Instruction->accept(*this);
  }

  Builder.CreateCondBr(CondV, LoopBB, AfterLoopBB);
  F->getBasicBlockList().push_back(AfterLoopBB);
  Builder.SetInsertPoint(AfterLoopBB);
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
  auto *Type = [this, X]() -> llvm::Type * {
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
      case ast::CLengthKind::CLK_Short:
        return Builder.getInt16Ty();
      case ast::CLengthKind::CLK_Default:
      case ast::CLengthKind::CLK_Long:
        return Builder.getInt32Ty();
      case ast::CLengthKind::CLK_LongLong:
        return Builder.getInt64Ty();
      }
    }

    return nullptr;
  }();

  for (unsigned int Index = 0; Index < X.Pointer; ++Index) {
    Type = Type->getPointerTo();
  }

  return Type;
}

llvm::Value *IRGenerator::greaterThan(llvm::Value *Left, llvm::Value *Right) {
  if (Left->getType()->isIntegerTy() && Right->getType()->isIntegerTy()) {
    return Builder.CreateICmpSGT(Left, Right);
  } else if ((Left->getType()->isDoubleTy() &&
              Right->getType()->isDoubleTy()) ||
             (Left->getType()->isFloatTy() && Right->getType()->isFloatTy())) {
    return Builder.CreateFCmpOGT(Left, Right);
  } else {
    throw CodeGenException("Invalid greater than op.");
  }
}

llvm::Value *IRGenerator::equals(llvm::Value *Left, llvm::Value *Right) {
  if (Left->getType()->isIntegerTy() && Right->getType()->isIntegerTy()) {
    return Builder.CreateICmpEQ(Left, Right);
  } else if ((Left->getType()->isDoubleTy() &&
              Right->getType()->isDoubleTy()) ||
             (Left->getType()->isFloatTy() && Right->getType()->isFloatTy())) {
    return Builder.CreateFCmpOEQ(Left, Right);
  } else {
    throw CodeGenException("Invalid equals op.");
  }
}

llvm::Value *IRGenerator::assign(llvm::Value *Left, llvm::Value *Right) {
  return Builder.CreateStore(Right, Left);
}

llvm::Value *IRGenerator::add(llvm::Value *Left, llvm::Value *Right) {
  if (Left->getType()->isIntegerTy() && Right->getType()->isIntegerTy()) {
    return Builder.CreateAdd(Left, Right);
  } else if ((Left->getType()->isDoubleTy() &&
              Right->getType()->isDoubleTy()) ||
             (Left->getType()->isFloatTy() && Right->getType()->isFloatTy())) {
    return Builder.CreateFAdd(Left, Right);
  } else {
    throw CodeGenException("Invalid add.");
  }
}

} // namespace fantac::codegen
