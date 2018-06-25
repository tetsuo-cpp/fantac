#pragma once

#include <memory>

namespace fantac::ast {

// Fwd declare all AST nodes.
struct FunctionNode;

// Visitor interface for walking the AST.
class IASTVisitor {
public:
  virtual ~IASTVisitor() = default;

  virtual void visit(FunctionNode *AST) = 0;
};

// Base class for all AST nodes.
struct IASTNode {
  virtual ~IASTNode() = default;

  virtual void accept(IASTVisitor *Visitor) = 0;
};

using ASTPtr = std::unique_ptr<IASTNode>;

enum class ValueKind {
  VK_Int,
  VK_UInt,
  VK_Long,
  VK_ULong,
  VK_Float,
  VK_Double,
  VK_Char
};

struct FunctionNode : public IASTNode {
  template <typename TName, typename TArgs>
  FunctionNode(TName &&Name, ValueKind Return, TArgs &&Args,
               std::vector<ASTPtr> &&Body)
      : Name(std::forward<TName>(Name)), Return(Return),
        Args(std::forward<TArgs>(Args)), Body(std::move(Body)) {}

  virtual void accept(IASTVisitor *Visitor) override { Visitor->visit(this); }

  template <typename TStream>
  friend TStream &operator<<(TStream &Stream, const FunctionNode &F);

  const std::string Name;
  ValueKind Return;
  const std::vector<std::pair<std::string, ValueKind>> Args;
  const std::vector<ASTPtr> Body;
};

template <typename TStream>
TStream &operator<<(TStream &Stream, const FunctionNode &F) {
  Stream << "{Name=" << F.Name << ", Args=";
  for (const auto &Args : F.Args) {
    Stream << Args.first << ",";
  }

  Stream << "}";
  return Stream;
}

} // namespace fantac::ast
