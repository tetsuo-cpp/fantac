#pragma once

#include "AST.h"

#include <cassert>
#include <iostream>

namespace fantac::ast {

class ASTLogger : public IASTVisitor {
public:
  virtual ~ASTLogger() = default;

  // IASTVisitor impl.
  virtual void visit(FunctionDecl *AST) override;
  virtual void visit(FunctionDef *AST) override;
};

inline void ASTLogger::visit(FunctionDecl *AST) {
  assert(AST);
  std::cout << "FunctionDecl: " << *AST << ".\n";
}

inline void ASTLogger::visit(FunctionDef *AST) {
  assert(AST);
  std::cout << "FunctionDef: " << *AST << ".\n";
}

} // namespace fantac::ast
