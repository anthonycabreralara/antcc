#include "ast.h"
#include "utils.h"
#include <iostream>

// -------- AST Node Constructors --------
ReturnNode::ReturnNode(std::unique_ptr<Node> e) {
    type = NodeType::RETURN;
    expr = std::move(e);
}

Complement::Complement() {
    type = NodeType::COMPLEMENT;
}

Negate::Negate() {
    type = NodeType::NEGATE;
}

UnOpNode::UnOpNode(std::unique_ptr<Node> o, std::unique_ptr<Node> e) {
    type = NodeType::UNARY_OP;
    op = std::move(o);
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
            printSpace(count);
            std::cout << "Function(" << std::endl;
            printSpace(count + 3);
            std::cout << "name=" << functionNode->name << std::endl;
            printSpace(count + 3);
            std::cout << "body=";
            printAST(functionNode->statement.get(), count + 3);
            printSpace(count);
            std::cout << ")" << std::endl;
            break;
        }
        case NodeType::RETURN: {
            const ReturnNode* returnNode = static_cast<const ReturnNode*>(node);
            std::cout << "Return(" << std::endl;
            printAST(returnNode->expr.get(), count + 3);
            printSpace(count);
            std::cout << ")" << std::endl;
            break;
        }
        case NodeType::CONSTANT: {
            const ConstantNode* constantNode = static_cast<const ConstantNode*>(node);
            printSpace(count);
            std::cout << "Constant(" << constantNode->value << ")" << std::endl;
            break;
        }
        case NodeType::UNARY_OP: {
            const UnOpNode* unOpNode = static_cast<const UnOpNode*>(node);
            printSpace(count);
            std::cout << "UnOp(";
            printAST(unOpNode->op.get(), 0);
            std::cout << ", " << std::endl;
            printAST(unOpNode->expr.get(), count + 3);
            printSpace(count);
            std::cout << ")" << std::endl;
            break;
        }
        case NodeType::NEGATE: {
            std::cout << "Negate";
            break;
        }
        case NodeType::COMPLEMENT: {
            std::cout << "Compliment";
            break;
        }
    }
}