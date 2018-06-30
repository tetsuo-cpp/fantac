#pragma once

#include "AST.h"

#include <cassert>
#include <iostream>

namespace fantac::ast {

class ASTLogger : public IASTVisitor {
public:
  virtual ~ASTLogger() = default;

  // IASTVisitor impl.
  virtual void visit(FunctionNode *AST) override;
};

inline void ASTLogger::visit(FunctionNode *AST) {
  assert(AST);
  std::cout << "FunctionNode: " << *AST << ".\n";
}

} // namespace fantac::ast
