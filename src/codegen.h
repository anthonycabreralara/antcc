#ifndef CODEGEN_H
#define CODEGEN_H
#include "ast.h"
#include "ir.h"

std::unique_ptr<IRNode> generateCode(const Node* node);
void printIR(const IRNode* node, int space);

#endif