#ifndef CODEGEN_H
#define CODEGEN_H
#include "ast.h"
#include "asm_ir.h"

std::unique_ptr<AsmIRNode> generateCode(const Node* node);
void printIR(const AsmIRNode* node, int space);

#endif