#ifndef GENERATE_TACKY_H
#define GENERATE_TACKY_H
#include "ast.h"
#include "tacky_ir.h"
#include "asm_ir.h"

std::unique_ptr<TackyIRNode> generateTacky(const Node* node);
void printTacky(const TackyIRNode* node, int count);

#endif