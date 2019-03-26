#pragma once

#include "ASTInterfaces.h"

#include <Parse/Token.h>

#include <vector>

namespace fantac::ast {

enum class CTypeKind {
  CTK_Int,
  CTK_Float,
  CTK_Double,
  CTK_Char,
  CTK_Void,
};

enum class CLengthKind {
  CLK_Default,
  CLK_Short,
  CLK_Long,
  CLK_LongLong,
};

struct CType {
  CType(CTypeKind Type, CLengthKind Length, bool Signed, unsigned int Pointer)
      : Type(Type), Length(Length), Signed(Signed), Pointer(Pointer) {}

  const CTypeKind Type;
  const CLengthKind Length;
  const bool Signed;
  const unsigned int Pointer;
};

inline const char *cTypeKindToString(CTypeKind Kind) {
  switch (Kind) {
  case CTypeKind::CTK_Int:
    return "int";
  case CTypeKind::CTK_Float:
    return "float";
  case CTypeKind::CTK_Double:
    return "double";
  case CTypeKind::CTK_Char:
    return "char";
  case CTypeKind::CTK_Void:
    return "void";
  }

  return "UNKNOWN";
}

inline const char *cLengthKindToString(CLengthKind Kind) {
  switch (Kind) {
  case CLengthKind::CLK_Short:
    return "short ";
  case CLengthKind::CLK_Long:
    return "long ";
  case CLengthKind::CLK_LongLong:
    return "long long ";
  case CLengthKind::CLK_Default:
    return "";
  }

  return "UNKNOWN ";
}

inline std::string cTypeToString(const CType &Type) {
  return fmt::format("{}{}{}{}", Type.Signed ? "" : "unsigned ",
                     cLengthKindToString(Type.Length),
                     cTypeKindToString(Type.Type),
                     std::string('*', Type.Pointer));
}

struct FunctionDecl : public IAST {
  template <typename T0, typename T1>
  FunctionDecl(T0 &&Name, CType Return, T1 &&Args)
      : Name(std::forward<T0>(Name)), Return(Return),
        Args(std::forward<T1>(Args)) {}

  // IAST impl.
  void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }

  std::string toString() const override {
    std::string ArgString;
    for (const auto &Arg : Args) {
      ArgString.append(
          fmt::format("{} {}", cTypeToString(Arg.second), Arg.first));
      if (&Arg != &Args.back())
        ArgString.append(", ");
    }

    return fmt::format("{} {}({})", cTypeToString(Return), Name, ArgString);
  }

  const std::string Name;
  const CType Return;
  const std::vector<std::pair<std::string, CType>> Args;
};

struct FunctionDef : public IAST {
  FunctionDef(std::unique_ptr<FunctionDecl> Decl, std::vector<ASTPtr> &&Body)
      : Decl(std::move(Decl)), Body(std::move(Body)) {}

  template <typename T0, typename T1>
  FunctionDef(T0 &&Name, CType Return, T1 &&Args, std::vector<ASTPtr> &&Body)
      : Decl(new FunctionDecl(std::forward<T0>(Name), Return,
                              std::forward<T1>(Args))),
        Body(std::move(Body)) {}

  // IAST impl.
  void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }

  std::string toString() const override {
    std::string BodyString;
    for (const auto &B : Body)
      BodyString.append(fmt::format("{};\n", B->toString()));

    return fmt::format("{} {{\n{}}}", Decl->toString(), BodyString);
  }

  const std::unique_ptr<FunctionDecl> Decl;
  const std::vector<ASTPtr> Body;
};

struct VariableDecl : public IAST {
  template <typename T>
  VariableDecl(CType Type, T &&Name, ASTPtr AssignmentExpr = nullptr)
      : Type(Type), Name(std::forward<T>(Name)),
        AssignmentExpr(std::move(AssignmentExpr)) {}

  // IAST impl.
  void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }

  std::string toString() const override {
    if (AssignmentExpr) {
      return fmt::format("{} {} = {}", cTypeToString(Type), Name,
                         AssignmentExpr->toString());
    }

    return fmt::format("{} {}", cTypeToString(Type), Name);
  }

  const CType Type;
  const std::string Name;
  const ASTPtr AssignmentExpr;
};

struct UnaryOp : public IAST {
  UnaryOp(parse::TokenKind Operator, ASTPtr Expr)
      : Operator(Operator), Expr(std::move(Expr)) {}

  // IAST impl.
  void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }

  std::string toString() const override {
    return fmt::format("{}({})", parse::tokenKindToString(Operator),
                       Expr->toString());
  }

  const parse::TokenKind Operator;
  const ASTPtr Expr;
};

struct BinaryOp : public IAST {
  BinaryOp(parse::TokenKind Operator, ASTPtr Left, ASTPtr Right)
      : Operator(Operator), Left(std::move(Left)), Right(std::move(Right)) {}

  // IAST impl.
  void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }

  std::string toString() const override {
    return fmt::format("({}) {} ({})", Left->toString(),
                       parse::tokenKindToString(Operator), Right->toString());
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
  void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); };

  std::string toString() const override {
    std::string ThenString;
    for (const auto &T : Then)
      ThenString.append(fmt::format("{};\n", T->toString()));

    if (!Else.empty()) {
      std::string ElseString;
      for (const auto &E : Else)
        ElseString.append(fmt::format("{}\n", E->toString()));

      return fmt::format("if ({}) {{\n{}}} else {{\n{}}}",
                         Condition->toString(), ThenString, ElseString);
    }

    return fmt::format("if ({}) {{\n{}}}", Condition->toString(), ThenString);
  }

  const ASTPtr Condition;
  const std::vector<ASTPtr> Then, Else;
};

struct TernaryCond : public IAST {
  TernaryCond(ASTPtr Condition, ASTPtr Then, ASTPtr Else)
      : Condition(std::move(Condition)), Then(std::move(Then)),
        Else(std::move(Else)) {}

  // IAST impl.
  void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }

  std::string toString() const override {
    return fmt::format("() ? () : ()", Condition->toString(), Then->toString(),
                       Else->toString());
  }

  const ASTPtr Condition, Then, Else;
};

struct WhileLoop : public IAST {
  WhileLoop(ASTPtr Condition, std::vector<ASTPtr> &&Body)
      : Condition(std::move(Condition)), Body(std::move(Body)) {}

  // IAST impl.
  void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }

  std::string toString() const override {
    std::string BodyString;
    for (const auto &B : Body)
      BodyString.append(fmt::format("{};\n", B->toString()));

    return fmt::format("while ({}) {{\n{}}}", Condition->toString(),
                       BodyString);
  }

  const ASTPtr Condition;
  const std::vector<ASTPtr> Body;
};

struct ForLoop : public IAST {
  ForLoop(ASTPtr Init, ASTPtr Condition, ASTPtr Iteration,
          std::vector<ASTPtr> &&Body)
      : Init(std::move(Init)), Condition(std::move(Condition)),
        Iteration(std::move(Iteration)), Body(std::move(Body)) {}

  // IAST impl.
  void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }

  std::string toString() const override {
    std::string BodyString;
    for (const auto &B : Body)
      BodyString.append(fmt::format("{};\n", B->toString()));

    return fmt::format("for (({});({});({})) {\n{}}", Init->toString(),
                       Condition->toString(), Iteration->toString(),
                       BodyString);
  }

  const ASTPtr Init, Condition, Iteration;
  const std::vector<ASTPtr> Body;
};

struct IntegerLiteral : public IAST {
  explicit IntegerLiteral(unsigned int Value) : Value(Value) {}

  // IAST impl.
  void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }
  std::string toString() const override { return fmt::format("{}", Value); }

  const unsigned int Value;
};

struct FloatLiteral : public IAST {
  explicit FloatLiteral(double Value) : Value(Value) {}

  // IAST impl.
  void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }
  std::string toString() const override { return fmt::format("{}", Value); }

  const double Value;
};

struct CharLiteral : public IAST {
  explicit CharLiteral(char Value) : Value(Value) {}

  // IAST impl.
  void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }
  std::string toString() const override { return fmt::format("\'{}\'", Value); }

  const char Value;
};

struct StringLiteral : public IAST {
  template <typename T>
  explicit StringLiteral(T &&Value) : Value(std::forward<T>(Value)) {}

  // IAST impl.
  void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }
  std::string toString() const override { return fmt::format("\"{}\"", Value); }

  const std::string Value;
};

struct VariableRef : public IAST {
  template <typename T>
  explicit VariableRef(T &&Name) : Name(std::forward<T>(Name)) {}

  // IAST impl.
  void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }
  std::string toString() const override { return Name; }

  const std::string Name;
};

struct MemberAccess : public IAST {
  template <typename T>
  MemberAccess(ASTPtr Expr, T &&MemberName)
      : Expr(std::move(Expr)), MemberName(std::forward<T>(MemberName)) {}

  // IAST impl.
  void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }

  std::string toString() const override {
    return fmt::format("{}.{}", Expr->toString(), MemberName);
  }

  const ASTPtr Expr;
  const std::string MemberName;
};

struct FunctionCall : public IAST {
  template <typename T>
  FunctionCall(T &&Name, std::vector<ASTPtr> &&Args)
      : Name(std::forward<T>(Name)), Args(std::move(Args)) {}

  // IAST impl.
  void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }

  std::string toString() const override {
    std::string ArgString;
    for (const auto &Arg : Args) {
      ArgString.append(Arg->toString());
      if (Arg.get() != Args.back().get())
        ArgString.append(", ");
    }

    return fmt::format("{}({})", Name, ArgString);
  }

  const std::string Name;
  const std::vector<ASTPtr> Args;
};

struct Return : public IAST {
  explicit Return(ASTPtr Expr) : Expr(std::move(Expr)) {}

  // IAST impl.
  void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }

  std::string toString() const override {
    return fmt::format("return {}", Expr->toString());
  }

  const ASTPtr Expr;
};

} // namespace fantac::ast
