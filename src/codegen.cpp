#include "codegen.h"
#include "parser.h"
#include <iostream>
#include <fstream>



void generate(Node* node, std::ofstream& outf) {
  if (node->type == Node::PROGRAM) {
    ProgramNode* programNode = static_cast<ProgramNode*>(node);
    generate(programNode->function, outf);
  }
  
  if (node->type == Node::FUNCTION) {
    FunctionNode* functionNode = static_cast<FunctionNode*>(node);
    outf << "_" << functionNode->name << "\n";
    generate(functionNode->statement, outf);
  }

  if (node->type == Node::RETURN) {
    ReturnNode* returnNode = static_cast<ReturnNode*>(node);
    generate(returnNode->expr, outf);
    outf << "ret";
  }

  if (node->type == Node::CONSTANT) {
    ConstantNode* constantNode = static_cast<ConstantNode*>(node);
    outf << "\tmovl " << "$" << constantNode->value << ", %eax\n";
  }
}

int generateCode(Node* node) {
  std::ofstream outf{ "output.s" };
  if (!outf) {
    std::cerr << "Uh oh, output.s could not be opened for reading!\n";
    return 1;
  }
  generate(node, outf);
  return 0;
}