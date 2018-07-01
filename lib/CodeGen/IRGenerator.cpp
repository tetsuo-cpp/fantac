#include "IRGenerator.h"

namespace fantac::codegen {

IRGenerator::IRGenerator() : Builder(Context), Module("FantaC", Context) {}

void IRGenerator::visit(ast::FunctionDecl *AST) { static_cast<void>(AST); }

void IRGenerator::visit(ast::FunctionDef *AST) { static_cast<void>(AST); }

} // namespace fantac::codegen
