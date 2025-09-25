// main.cpp
#include "Lexer.h"
#include "Parser.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <cstdio>
#include <memory>
#include <array>

std::string readFileToString(const std::string &filename) {
  std::string cmd = "gcc -E -P " + filename; // or "clang -E -P " + filename
  std::array<char, 128> buffer;
  std::string result;

  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
  if (!pipe) throw std::runtime_error("popen() failed!");

  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    result += buffer.data();
  }

  return result;
}

void printTabs(int count) {
  for (int i = 0; i < count; i++) {
    std::cout << ' ';
  }
}
// Return(Constant(2))
void printAST(Node* node, int count) {
  if (node->type == Node::PROGRAM) {
    ProgramNode* programNode = static_cast<ProgramNode*>(node);
    std::cout << "Program(" << std::endl;
    printAST(programNode->function, count + 3);
    std::cout << ")" << std::endl;
  }
  
  if (node->type == Node::FUNCTION) {
    FunctionNode* functionNode = static_cast<FunctionNode*>(node);
    printTabs(count);
    std::cout << "Function(" << std::endl;
    printTabs(count + 3);
    std::cout << "name=" << functionNode->name << std::endl;
    printTabs(count + 3);
    std::cout << "body=";
    printAST(functionNode->statement, count + 3);
    printTabs(count);
    std::cout << ")" << std::endl;
  }

  if (node->type == Node::RETURN) {
    ReturnNode* returnNode = static_cast<ReturnNode*>(node);
    std::cout << "Return(" << std::endl;
    printAST(returnNode->expr, count + 3);
    printTabs(count);
    std::cout << ")" << std::endl;

  }

  if (node->type == Node::CONSTANT) {
    ConstantNode* constantNode = static_cast<ConstantNode*>(node);
    printTabs(count);
    std::cout << "Constant(" << constantNode->value << ")" << std::endl;
  }

}


int main(int argc, char *argv[]) {

  if (argc < 2) {
    std::cout << "Need filepath." << std::endl;
    return 1;
  }

  std::string sourceCodeFilepath = argv[1];

  std::string sourceCode = readFileToString(sourceCodeFilepath);
  Lexer lexer(sourceCode);
  std::vector<Token> tokens = lexer.tokenize();

  Parser parser(tokens);
  Node* AST = parser.parse();
  printAST(AST, 0);
  return 0;
}
