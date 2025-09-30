#include "ast.h"
#include <iostream>

// -------- AST Node Constructors --------
ReturnNode::ReturnNode(std::unique_ptr<Node> e) {
    type = NodeType::RETURN;
    expr = std::move(e);
}

UnOpNode::UnOpNode(std::string o, std::unique_ptr<Node> e) {
    type = NodeType::UNARY_OP;
    op = o;
    expr = std::move(e);
}

ConstantNode::ConstantNode(std::string v) {
    type = NodeType::CONSTANT;
    value = std::move(v);
}

FunctionNode::FunctionNode(std::string n, int r, std::unique_ptr<Node> stmt) {
    type = NodeType::FUNCTION;
    name = std::move(n);
    returnType = r;
    statement = std::move(stmt);
}

ProgramNode::ProgramNode(std::unique_ptr<FunctionNode> func) {
    type = NodeType::PROGRAM;
    function = std::move(func);
}

// -------- AST Printer --------

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
        case NodeType::UNARY_OP: {
            const UnOpNode* unOpNode = static_cast<const UnOpNode*>(node);
            printTabs(count);
            std::cout << "UnOp(" << unOpNode->op << ", " << std::endl;
            printAST(unOpNode->expr.get(), count + 3);
            printTabs(count);
            std::cout << ")" << std::endl;
        }
    }
}