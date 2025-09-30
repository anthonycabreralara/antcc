#include "lexer.h"
#include "parser.h"
#include "codegen.h"
#include "ir.h"
#include "emitter.h"
#include "ast.h"
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

void printAST(const Node* node, int count) {
    if (!node) return;

    switch (node->type) {
        case NodeType::PROGRAM: {
            const ProgramNode* programNode = static_cast<const ProgramNode*>(node);
            std::cout << "Program(" << std::endl;
            printAST(programNode->function.get(), count + 3);
            std::cout << ")" << std::endl;
            break;
        }
        case NodeType::FUNCTION: {
            const FunctionNode* functionNode = static_cast<const FunctionNode*>(node);
            printTabs(count);
            std::cout << "Function(" << std::endl;
            printTabs(count + 3);
            std::cout << "name=" << functionNode->name << std::endl;
            printTabs(count + 3);
            std::cout << "body=";
            printAST(functionNode->statement.get(), count + 3);
            printTabs(count);
            std::cout << ")" << std::endl;
            break;
        }
        case NodeType::RETURN: {
            const ReturnNode* returnNode = static_cast<const ReturnNode*>(node);
            std::cout << "Return(" << std::endl;
            printAST(returnNode->expr.get(), count + 3);
            printTabs(count);
            std::cout << ")" << std::endl;
            break;
        }
        case NodeType::CONSTANT: {
            const ConstantNode* constantNode = static_cast<const ConstantNode*>(node);
            printTabs(count);
            std::cout << "Constant(" << constantNode->value << ")" << std::endl;
            break;
        }
    }
}

int main(int argc, char *argv[]) {
    std::string option = "";
    if (argc < 2) {
        std::cout << "Need filepath." << std::endl;
        return 1;
    } else if (argc > 3) {
        std::cout << "Too many arguments." << std::endl;
        return 1;
    } else if (argc == 3) {
        option = argv[2];
        if (option != "--lex" && option != "--parse" && option != "--codegen") {
            std::cout << "Invalid options." << std::endl;
            return 1;
        }
    }

    std::string sourceCodeFilepath = argv[1];

    std::string sourceCode = readFileToString(sourceCodeFilepath);

    Lexer lexer(sourceCode);
    std::vector<Token> tokens = lexer.tokenize();

    if (option == "--lex") {
        printTokens(tokens);
        return 0;
    }

    Parser parser(tokens);
    auto AST = parser.parse();  // unique_ptr<Node>

    if (option == "--parse") {
        printAST(AST.get(), 0);  // pass raw pointer for printing
        return 0;
    }

    auto ir = generateCode(AST.get());

    if (option == "-codegen") {
        return 0;
    }

    emitCode(ir.get());





    return 0;
}
