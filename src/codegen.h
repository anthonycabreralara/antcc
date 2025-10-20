#ifndef CODEGEN_H
#define CODEGEN_H
#include "tacky_ir.h"
#include "asm_ir.h"

std::unique_ptr<AsmIRNode> generateCode(const TackyIRNode* node, AsmIRInstructions* instructions);
void printIR(const AsmIRNode* node, int space);
std::unique_ptr<AsmIRInstructions> unnestInstructions(std::unique_ptr<AsmIRInstructions> node);

#endif