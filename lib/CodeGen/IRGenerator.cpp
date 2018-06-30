#include "IRGenerator.h"

namespace fantac::codegen {

IRGenerator::IRGenerator() : Builder(Context), Module("fantac", Context) {}

void IRGenerator::visit(ast::FunctionNode *AST) { static_cast<void>(AST); }

} // namespace fantac::codegen
