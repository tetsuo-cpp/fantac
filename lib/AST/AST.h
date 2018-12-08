#pragma once

#include <Parse/Token.h>

#include "ASTInterfaces.h"

#include <vector>

namespace fantac::ast {

enum class CTypeKind {
  CTK_Int,
  CTK_Long,
  CTK_Float,
  CTK_Double,
  CTK_Char,
  CTK_None
};

struct FunctionDecl : public IAST {
  template <typename T0, typename T1>
  FunctionDecl(T0 &&Name, CTypeKind Return, T1 &&Args)
      : Name(std::forward<T0>(Name)), Return(Return),
        Args(std::forward<T1>(Args)) {}

  // IAST impl.
  virtual void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }

  template <typename T> friend T &operator<<(T &Stream, const FunctionDecl &F) {
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

  const std::string Name;
  const CTypeKind Return;
  const std::vector<std::pair<std::string, CTypeKind>> Args;
};

struct FunctionDef : public IAST {
  FunctionDef(std::unique_ptr<FunctionDecl> &&Decl, std::vector<ASTPtr> &&Body)
      : Decl(std::move(Decl)), Body(std::move(Body)) {}

  template <typename T0, typename T1>
  FunctionDef(T0 &&Name, CTypeKind Return, T1 &&Args,
              std::vector<ASTPtr> &&Body)
      : Decl(new FunctionDecl(std::forward<T0>(Name), Return,
                              std::forward<T1>(Args))),
        Body(std::move(Body)) {}

  // IAST impl.
  virtual void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }

  template <typename T> friend T &operator<<(T &Stream, const FunctionDef &F) {
    Stream << "{Decl=" << *F.Decl << "}";
    return Stream;
  }

  const std::unique_ptr<FunctionDecl> Decl;
  const std::vector<ASTPtr> Body;
};

struct VariableDecl : public IAST {
  template <typename T>
  VariableDecl(CTypeKind Type, T &&Name, ASTPtr &&AssignmentExpr = nullptr)
      : Type(Type), Name(std::forward<T>(Name)),
        AssignmentExpr(std::move(AssignmentExpr)) {}

  // IAST impl.
  virtual void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }

  template <typename T> friend T &operator<<(T &Stream, const VariableDecl &V) {
    Stream << "{Name=" << V.Name << "}";
    return Stream;
  }

  const CTypeKind Type;
  const std::string Name;
  const ASTPtr AssignmentExpr;
};

struct UnaryOp : public IAST {
  UnaryOp(parse::TokenKind Operator, ASTPtr &&Expr)
      : Operator(Operator), Expr(std::move(Expr)) {}

  // IAST impl.
  virtual void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }

  template <typename T> friend T &operator<<(T &Stream, const UnaryOp &U) {
    Stream << "{Operator=" << parse::tokenKindToString(U.Operator) << "}";
    return Stream;
  }

  const parse::TokenKind Operator;
  const ASTPtr Expr;
};

struct BinaryOp : public IAST {
  BinaryOp(parse::TokenKind Operator, ASTPtr &&Left, ASTPtr &&Right)
      : Operator(Operator), Left(std::move(Left)), Right(std::move(Right)) {}

  // IAST impl.
  virtual void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }

  template <typename T> friend T &operator<<(T &Stream, const BinaryOp &B) {
    Stream << "{Operator=" << parse::tokenKindToString(B.Operator) << "}";
    return Stream;
  }

  const parse::TokenKind Operator;
  const ASTPtr Left, Right;
};

struct IfCond : public IAST {
  IfCond(ASTPtr &&Condition, std::vector<ASTPtr> &&Then,
         std::vector<ASTPtr> &&Else)
      : Condition(std::move(Condition)), Then(std::move(Then)),
        Else(std::move(Else)) {}

  // IAST impl.
  virtual void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); };

  const ASTPtr Condition;
  const std::vector<ASTPtr> Then, Else;
};

struct TernaryCond : public IAST {
  TernaryCond(ASTPtr &&Condition, ASTPtr &&Then, ASTPtr &&Else)
      : Condition(std::move(Condition)), Then(std::move(Then)),
        Else(std::move(Else)) {}

  // IAST impl.
  virtual void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }

  const ASTPtr Condition, Then, Else;
};

struct WhileLoop : public IAST {
  WhileLoop(ASTPtr &&Condition, std::vector<ASTPtr> &&Body)
      : Condition(std::move(Condition)), Body(std::move(Body)) {}

  // IAST impl.
  virtual void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }

  const ASTPtr Condition;
  const std::vector<ASTPtr> Body;
};

struct ForLoop : public IAST {
  ForLoop(ASTPtr &&Init, ASTPtr &&Condition, ASTPtr &&Iteration,
          std::vector<ASTPtr> &&Body)
      : Init(std::move(Init)), Condition(std::move(Condition)),
        Iteration(std::move(Iteration)), Body(std::move(Body)) {}

  // IAST impl.
  virtual void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }

  const ASTPtr Init, Condition, Iteration;
  const std::vector<ASTPtr> Body;
};

struct NumberLiteral : public IAST {
  explicit NumberLiteral(unsigned int Value) : Value(Value) {}

  // IAST impl.
  virtual void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }

  template <typename T>
  friend T &operator<<(T &Stream, const NumberLiteral &N) {
    Stream << "{Value=" << N.Value << "}";
    return Stream;
  }

  const unsigned int Value;
};

struct StringLiteral : public IAST {
  template <typename T>
  explicit StringLiteral(T &&Value) : Value(std::forward<T>(Value)) {}

  // IAST impl.
  virtual void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }

  template <typename T>
  friend T &operator<<(T &Stream, const StringLiteral &S) {
    Stream << "{Value=" << S.Value << "}";
    return Stream;
  }

  const std::string Value;
};

struct VariableRef : public IAST {
  template <typename T>
  explicit VariableRef(T &&Name) : Name(std::forward<T>(Name)) {}

  // IAST impl.
  virtual void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }

  template <typename T> friend T &operator<<(T &Stream, const VariableRef &V) {
    Stream << "{Name=" << V.Name << "}";
    return Stream;
  }

  const std::string Name;
};

struct MemberAccess : public IAST {
  template <typename T>
  MemberAccess(ASTPtr &&Expr, T &&MemberName)
      : Expr(std::move(Expr)), MemberName(std::forward<T>(MemberName)) {}

  // IAST impl.
  virtual void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }

  template <typename T> friend T &operator<<(T &Stream, const MemberAccess &M) {
    Stream << "{MemberName=" << M.MemberName << "}";
    return Stream;
  }

  const ASTPtr Expr;
  const std::string MemberName;
};

struct FunctionCall : public IAST {
  template <typename T>
  FunctionCall(T &&Name, std::vector<ASTPtr> &&Args)
      : Name(std::forward<T>(Name)), Args(std::move(Args)) {}

  // IAST impl.
  virtual void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }

  template <typename T> friend T &operator<<(T &Stream, const FunctionCall &F) {
    Stream << "{Name=" << F.Name << "}";
    return Stream;
  }

  const std::string Name;
  const std::vector<ASTPtr> Args;
};

} // namespace fantac::ast
