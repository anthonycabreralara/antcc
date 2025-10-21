#ifndef CODEGEN_H
#define CODEGEN_H
#include "tacky_ir.h"
#include "asm_ir.h"
#include <unordered_map>

std::unique_ptr<AsmIRNode> generateCode(const TackyIRNode* node);
void printIR(const AsmIRNode* node, int space);
std::unique_ptr<AsmIRInstructions> unnestInstructions(std::unique_ptr<AsmIRInstructions> node);
void replacePseudoLoop(AsmIRNode* node, std::unordered_map<std::string, int>& pseudoToOffset, int& nextOffset);
std::unique_ptr<AsmIRNode> buildAsmIRAst(const TackyIRNode* node, AsmIRInstructions* instructions);
#endif