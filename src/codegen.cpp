#include "codegen.h"
#include "ir.h"
#include "ast.h"
#include <iostream>



std::unique_ptr<IRNode> generateCode(const Node* node) {
    switch (node->type) {
        case NodeType::PROGRAM: {
            const ProgramNode* programNode = static_cast<const ProgramNode*>(node);
            auto program = std::make_unique<IRProgram>(nullptr);
            generateCode(programNode->function.get());

            return std::move(program);
            break;
        }
        case NodeType::FUNCTION: {
            const FunctionNode* functionNode = static_cast<const FunctionNode*>(node);
            std::vector<std::unique_ptr<IRNode>> instructions;
            auto func = std::make_unique<IRFunction>(functionNode->name, std::move(instructions));
            generateCode(functionNode->statement.get());
            break;
        }
        case NodeType::RETURN: {
            const ReturnNode* returnNode = static_cast<const ReturnNode*>(node);
            auto dst = std::make_unique<IRReg>("eax");
            auto mov = std::make_unique<IRMov>(nullptr, nullptr);
            auto ret = std::make_unique<IRRet>();
            generateCode(returnNode->expr.get());
            break;
        }
        case NodeType::CONSTANT: {
            const ConstantNode* constantNode = static_cast<const ConstantNode*>(node);
            auto imm = std::make_unique<IRImm>(constantNode->value);
            break;
        }
        default:
            return nullptr;
    }
    
    return nullptr;
}