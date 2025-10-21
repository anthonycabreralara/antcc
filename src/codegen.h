#ifndef CODEGEN_H
#define CODEGEN_H
#include "tacky_ir.h"
#include "asm_ir.h"
#include <unordered_map>

std::unique_ptr<AsmIRNode> generateCode(const TackyIRNode* node);
void printIR(const AsmIRNode* node, int space);

#endif