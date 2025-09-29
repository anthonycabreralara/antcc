#include "codegen.h"
#include "ir.h"
#include "ast.h"
#include <iostream>



std::unique_ptr<IRNode> generateCode(const Node* node) {
    switch (node->type) {
        case NodeType::PROGRAM: {
            const ProgramNode* programNode = static_cast<const ProgramNode*>(node);
            auto program = generateCode(programNode->function.get());

            return std::move(program);
        }
        case NodeType::FUNCTION: {
            const FunctionNode* functionNode = static_cast<const FunctionNode*>(node);
            auto rawInstructions = generateCode(functionNode->statement.get());
            auto instructions = std::unique_ptr<IRInstructions>(static_cast<IRInstructions*>(rawInstructions.release()));
            auto func = std::make_unique<IRFunction>(functionNode->name, std::move(instructions));
            return func;
        }
        case NodeType::RETURN: {
            const ReturnNode* returnNode = static_cast<const ReturnNode*>(node);
            auto instructions = std::make_unique<IRInstructions>();
            auto expr = generateCode(returnNode->expr.get());
            auto dst = std::make_unique<IRReg>("eax");
            auto mov = std::make_unique<IRMov>(std::move(expr), std::move(dst));
            auto ret = std::make_unique<IRRet>();
            instructions->instructions.push_back(std::move(mov));
            instructions->instructions.push_back(std::move(ret));
            return std::move(instructions);
        }
        case NodeType::CONSTANT: {
            const ConstantNode* constantNode = static_cast<const ConstantNode*>(node);
            auto imm = std::make_unique<IRImm>(constantNode->value);
            return std::move(imm);
        }
        default:
            return nullptr;
    }

    return nullptr;
}