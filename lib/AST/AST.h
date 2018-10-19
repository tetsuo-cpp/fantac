#pragma once

#include <memory>
#include <vector>

namespace fantac::ast {

// Fwd declare all AST nodes.
struct FunctionDecl;
struct FunctionDef;
struct VariableDecl;

// Visitor interface for walking the AST.
class IASTVisitor {
public:
  virtual ~IASTVisitor() = default;

  virtual void visit(FunctionDecl &AST) = 0;
  virtual void visit(FunctionDef &AST) = 0;
  virtual void visit(VariableDecl &AST) = 0;
};

// Base class for all AST nodes.
struct IAST {
  virtual ~IAST() = default;

  virtual void accept(IASTVisitor &Visitor) = 0;
};

using ASTPtr = std::unique_ptr<IAST>;

enum class CTypeKind {
  CTK_Int,
  CTK_Long,
  CTK_Float,
  CTK_Double,
  CTK_Char,
  CTK_None
};

struct FunctionDecl : public IAST {
  template <typename TName, typename TArgs>
  FunctionDecl(TName &&Name, CTypeKind Return, TArgs &&Args)
      : Name(std::forward<TName>(Name)), Return(Return),
        Args(std::forward<TArgs>(Args)) {}

  // IAST impl.
  virtual void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }

  template <typename TStream>
  friend TStream &operator<<(TStream &Stream, const FunctionDecl &F);

  const std::string Name;
  CTypeKind Return;
  const std::vector<std::pair<std::string, CTypeKind>> Args;
};

struct FunctionDef : public IAST {
  FunctionDef(std::unique_ptr<FunctionDecl> &&Decl, std::vector<ASTPtr> &&Body)
      : Decl(std::move(Decl)), Body(std::move(Body)) {}

  template <typename TName, typename TArgs>
  FunctionDef(TName &&Name, CTypeKind Return, TArgs &&Args,
              std::vector<ASTPtr> &&Body)
      : Decl(new FunctionDecl(std::forward<TName>(Name), Return,
                              std::forward<TArgs>(Args))),
        Body(std::move(Body)) {}

  // IAST impl.
  virtual void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }

  template <typename TStream>
  friend TStream &operator<<(TStream &Stream, const FunctionDef &F);

  std::unique_ptr<FunctionDecl> Decl;
  const std::vector<ASTPtr> Body;
};

struct VariableDecl : public IAST {
  template <typename TName>
  VariableDecl(CTypeKind Type, TName &&Name)
      : Type(Type), Name(std::forward<TName>(Name)) {}

  // IAST impl.
  virtual void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }

  template <typename TStream>
  friend TStream &operator<<(TStream &Stream, const VariableDecl &V);

  const CTypeKind Type;
  const std::string Name;
  ast::ASTPtr ValueExpr;
};

template <typename TStream>
TStream &operator<<(TStream &Stream, const FunctionDecl &F) {
  Stream << "{Name=" << F.Name << ", Args=(";
  for (const auto &Arg : F.Args) {
    Stream << Arg.first;
    if (&Arg != &F.Args.back()) {
      Stream << ", ";
    }
  }

  Stream << ")}";
  return Stream;
}

template <typename TStream>
TStream &operator<<(TStream &Stream, const FunctionDef &F) {
  Stream << "{Decl=" << *F.Decl << "}";
  return Stream;
}

template <typename TStream>
TStream &operator<<(TStream &Stream, const VariableDecl &V) {
  Stream << "{Name=" << V.Name << "}";
  return Stream;
}

} // namespace fantac::ast
