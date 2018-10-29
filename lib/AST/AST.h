#pragma once

#include <memory>
#include <vector>

namespace fantac::ast {

// Fwd declare all AST nodes.
struct FunctionDecl;
struct FunctionDef;
struct VariableDecl;
struct BinaryOp;
struct IfCond;
struct NumberLiteral;
struct StringLiteral;
struct VariableRef;
struct WhileLoop;

// Visitor interface for walking the AST.
class IASTVisitor {
public:
  virtual ~IASTVisitor() = default;

  virtual void visit(FunctionDecl &AST) = 0;
  virtual void visit(FunctionDef &AST) = 0;
  virtual void visit(VariableDecl &AST) = 0;
  virtual void visit(BinaryOp &AST) = 0;
  virtual void visit(IfCond &AST) = 0;
  virtual void visit(NumberLiteral &AST) = 0;
  virtual void visit(StringLiteral &AST) = 0;
  virtual void visit(VariableRef &AST) = 0;
  virtual void visit(WhileLoop &AST) = 0;
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
  friend TStream &operator<<(TStream &Stream, const FunctionDecl &F) {
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

  template <typename TName, typename TArgs>
  FunctionDef(TName &&Name, CTypeKind Return, TArgs &&Args,
              std::vector<ASTPtr> &&Body)
      : Decl(new FunctionDecl(std::forward<TName>(Name), Return,
                              std::forward<TArgs>(Args))),
        Body(std::move(Body)) {}

  // IAST impl.
  virtual void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }

  template <typename TStream>
  friend TStream &operator<<(TStream &Stream, const FunctionDef &F) {
    Stream << "{Decl=" << *F.Decl << "}";
    return Stream;
  }

  const std::unique_ptr<FunctionDecl> Decl;
  const std::vector<ASTPtr> Body;
};

struct VariableDecl : public IAST {
  template <typename TName>
  VariableDecl(CTypeKind Type, TName &&Name)
      : Type(Type), Name(std::forward<TName>(Name)) {}

  // IAST impl.
  virtual void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }

  template <typename TStream>
  friend TStream &operator<<(TStream &Stream, const VariableDecl &V) {
    Stream << "{Name=" << V.Name << "}";
    return Stream;
  }

  const CTypeKind Type;
  const std::string Name;
  const ASTPtr ValueExpr;
};

struct BinaryOp : public IAST {
  BinaryOp(char Operator, ASTPtr &&Left, ASTPtr &&Right)
      : Operator(Operator), Left(std::move(Left)), Right(std::move(Right)) {}

  // IAST impl.
  virtual void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }

  template <typename TStream>
  friend TStream &operator<<(TStream &Stream, const BinaryOp &B) {
    Stream << "{Operator=" << B.Operator << "}";
    return Stream;
  }

  const char Operator;
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

struct WhileLoop : public IAST {
  WhileLoop(ASTPtr &&Condition, std::vector<ASTPtr> &&Body)
      : Condition(std::move(Condition)), Body(std::move(Body)) {}

  // IAST impl.
  virtual void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }

  const ASTPtr Condition;
  const std::vector<ASTPtr> Body;
};

struct NumberLiteral : public IAST {
  NumberLiteral(unsigned int Value) : Value(Value) {}

  // IAST impl.
  virtual void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }

  template <typename TStream>
  friend TStream &operator<<(TStream &Stream, const NumberLiteral &N) {
    Stream << "{Value=" << N.Value << "}";
    return Stream;
  }

  unsigned int Value;
};

struct StringLiteral : public IAST {
  template <typename TString>
  StringLiteral(TString &&Value) : Value(std::forward<TString>(Value)) {}

  // IAST impl.
  virtual void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }

  template <typename TStream>
  friend TStream &operator<<(TStream &Stream, const StringLiteral &S) {
    Stream << "{Value=" << S.Value << "}";
    return Stream;
  }

  const std::string Value;
};

struct VariableRef : public IAST {
  template <typename TString>
  VariableRef(TString &&Name) : Name(std::forward<TString>(Name)) {}

  // IAST impl.
  virtual void accept(IASTVisitor &Visitor) override { Visitor.visit(*this); }

  template <typename TStream>
  friend TStream &operator<<(TStream &Stream, const VariableRef &V) {
    Stream << "{Name=" << V.Name << "}";
    return Stream;
  }

  const std::string Name;
};

} // namespace fantac::ast
