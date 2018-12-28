#pragma once

#include <Parse/Token.h>

#include "ASTInterfaces.h"

#include <vector>

namespace fantac::ast {

enum class CTypeKind {
  CTK_Int,
  CTK_Float,
  CTK_Double,
  CTK_Char,
  CTK_Void,
  CTK_None
};

enum class CLengthKind {
  CLK_Default,
  CLK_Short,
  CLK_Long,
  CLK_LongLong,
  CLK_None
};

struct CType {
  CType(CTypeKind Type, CLengthKind Length, bool Signed, bool Pointer)
      : Type(Type), Length(Length), Signed(Signed), Pointer(Pointer) {}

  template <typename T> friend T &operator<<(T &Stream, const CType &X) {
    Stream << "{Type=";
    switch (X.Type) {
    case CTypeKind::CTK_Char:
      Stream << "Char";
      break;
    case CTypeKind::CTK_Int:
      Stream << "Int";
      break;
    case CTypeKind::CTK_Float:
      Stream << "Float";
      break;
    case CTypeKind::CTK_Double:
      Stream << "Double";
      break;
    case CTypeKind::CTK_Void:
      Stream << "Void";
      break;
    default:
      break;
    }

    Stream << ", Length=";
    switch (X.Length) {
    case CLengthKind::CLK_Default:
      Stream << "Default";
      break;
    case CLengthKind::CLK_Long:
      Stream << "Long";
      break;
    case CLengthKind::CLK_LongLong:
      Stream << "LongLong";
      break;
    default:
      break;
    }

    Stream << ", Signed=" << (X.Signed ? "true" : "false");
    Stream << ", Pointer=" << (X.Pointer ? "true" : "false");
    Stream << "}";
    return Stream;
  }

  CTypeKind Type;
  CLengthKind Length;
  bool Signed;
  bool Pointer;
};

struct FunctionDecl : public IAST {
  template <typename T0, typename T1>
  FunctionDecl(T0 &&Name, CType Return, T1 &&Args)
      : Name(std::forward<T0>(Name)), Return(Return),
        Args(std::forward<T1>(Args)) {}

  // IAST impl.
  virtual void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }

  template <typename T> friend T &operator<<(T &Stream, const FunctionDecl &F) {
    Stream << "{Name=" << F.Name << ", Return=" << F.Return << ", Args=(";
    for (const auto &Arg : F.Args) {
      Stream << "{Name=" << Arg.first << ", Type=" << Arg.second << "}";
      if (&Arg != &F.Args.back()) {
        Stream << ", ";
      }
    }

    Stream << ")}";
    return Stream;
  }

  const std::string Name;
  const CType Return;
  const std::vector<std::pair<std::string, CType>> Args;
};

struct FunctionDef : public IAST {
  FunctionDef(std::unique_ptr<FunctionDecl> &&Decl, std::vector<ASTPtr> &&Body)
      : Decl(std::move(Decl)), Body(std::move(Body)) {}

  template <typename T0, typename T1>
  FunctionDef(T0 &&Name, CType Return, T1 &&Args, std::vector<ASTPtr> &&Body)
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
  VariableDecl(CType Type, T &&Name, ASTPtr &&AssignmentExpr = nullptr)
      : Type(Type), Name(std::forward<T>(Name)),
        AssignmentExpr(std::move(AssignmentExpr)) {}

  // IAST impl.
  virtual void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }

  template <typename T> friend T &operator<<(T &Stream, const VariableDecl &V) {
    Stream << "{Name=" << V.Name << ", Type=" << V.Type << "}";
    return Stream;
  }

  const CType Type;
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

struct Return : public IAST {
  Return(ASTPtr &&Expr) : Expr(std::move(Expr)) {}

  // IAST impl.
  virtual void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }

  const ASTPtr Expr;
};

} // namespace fantac::ast
