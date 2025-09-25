#include "codegen.h"
#include "parser.h"
#include <iostream>

void generateCode(Node* node) {
  if (node->type == Node::PROGRAM) {
    ProgramNode* programNode = static_cast<ProgramNode*>(node);
  }
  
  if (node->type == Node::FUNCTION) {
    FunctionNode* functionNode = static_cast<FunctionNode*>(node);
  }

  if (node->type == Node::RETURN) {
    ReturnNode* returnNode = static_cast<ReturnNode*>(node);
  }

  if (node->type == Node::CONSTANT) {
    ConstantNode* constantNode = static_cast<ConstantNode*>(node);
  }
}