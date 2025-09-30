#include "codegen.h"
#include "ir.h"
#include "ast.h"
#include <iostream>

std::unique_ptr<IRNode> generateCode(const Node* node) {
    if (!node) return nullptr;

    switch (node->type) {
        case NodeType::PROGRAM: {
            const auto* programNode = static_cast<const ProgramNode*>(node);
            auto func = generateCode(programNode->function.get());

            return std::make_unique<IRProgram>(std::unique_ptr<IRFunction>(static_cast<IRFunction*>(func.release()))
            );
        }


        case NodeType::FUNCTION: {
            const auto* functionNode = static_cast<const FunctionNode*>(node);
            auto instructions = generateCode(functionNode->statement.get());

            return std::make_unique<IRFunction>(
                functionNode->name,
                std::unique_ptr<IRInstructions>(static_cast<IRInstructions*>(instructions.release()))
            );
        }

        case NodeType::RETURN: {
            const auto* returnNode = static_cast<const ReturnNode*>(node);
            auto instructions = std::make_unique<IRInstructions>();

            auto expr = generateCode(returnNode->expr.get());
            auto dst  = std::make_unique<IRReg>("eax");

            instructions->instructions.push_back(std::make_unique<IRMov>(std::move(expr), std::move(dst)));
            instructions->instructions.push_back(std::make_unique<IRRet>());

            return instructions;
        }

        case NodeType::CONSTANT: {
            const auto* constantNode = static_cast<const ConstantNode*>(node);
            return std::make_unique<IRImm>(constantNode->value);
        }

        default:
            return nullptr;
    }
}