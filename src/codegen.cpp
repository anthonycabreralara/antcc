#include "codegen.h"
#include "asm_ir.h"
#include "ast.h"
#include <iostream>

std::unique_ptr<AsmIRNode> generateCode(const Node* node) {
    if (!node) return nullptr;

    switch (node->type) {
        case NodeType::PROGRAM: {
            const auto* programNode = static_cast<const ProgramNode*>(node);
            auto func = generateCode(programNode->function.get());

            return std::make_unique<AsmIRProgram>(std::unique_ptr<AsmIRFunction>(static_cast<AsmIRFunction*>(func.release()))
            );
        }

        case NodeType::FUNCTION: {
            const auto* functionNode = static_cast<const FunctionNode*>(node);
            auto instructions = generateCode(functionNode->statement.get());

            return std::make_unique<AsmIRFunction>(
                functionNode->name,
                std::unique_ptr<AsmIRInstructions>(static_cast<AsmIRInstructions*>(instructions.release()))
            );
        }

        case NodeType::RETURN: {
            const auto* returnNode = static_cast<const ReturnNode*>(node);
            auto instructions = std::make_unique<AsmIRInstructions>();

            auto expr = generateCode(returnNode->expr.get());
            auto dst  = std::make_unique<AsmIRReg>("eax");

            instructions->instructions.push_back(std::make_unique<AsmIRMov>(std::move(expr), std::move(dst)));
            instructions->instructions.push_back(std::make_unique<AsmIRRet>());

            return instructions;
        }

        case NodeType::CONSTANT: {
            const auto* constantNode = static_cast<const ConstantNode*>(node);
            return std::make_unique<AsmIRImm>(constantNode->value);
        }

        default:
            return nullptr;
    }
}

void printSpace(int count) {
    for (int i = 0; i < count; i++) {
        std::cout << ' ';
    }
}

void printIR(const AsmIRNode* node, int space) {
    switch (node->type) {
        case AsmIRNodeType::PROGRAM: {
            std::cout << "program" << std::endl;
            const auto* programNode = static_cast<const AsmIRProgram*>(node);
            printIR(programNode->function.get(), space);
            break;
        }
        case AsmIRNodeType::FUNCTION: {
            const auto* functionNode = static_cast<const AsmIRFunction*>(node);
            std::cout << "\t.global " << functionNode->name << std::endl;
            std::cout << functionNode->name << ":" << std::endl;

            for (const auto& instr : functionNode->instructions->instructions) {
                printIR(instr.get(), space);
            }

            break;
        }
        case AsmIRNodeType::MOV: {
            const auto* moveNode = static_cast<const AsmIRMov*>(node);
            std::cout << "\tmovl ";
            printIR(moveNode->src.get(), space);
            std::cout << ", ";
            printIR(moveNode->dst.get(), space);
            std::cout << std::endl;
            break;
        }
        case AsmIRNodeType::IMMEDIATE: {
            const auto* immNode = static_cast<const AsmIRImm*>(node);
            std::cout << "$" << immNode->value;
            break;
        }
        case AsmIRNodeType::REGISTER: {
            const auto* regNode = static_cast<const AsmIRReg*>(node);
            std::cout << "%" << regNode->value;
            break;
        }
        case AsmIRNodeType::RETURN: {
            std::cout << "\tret" << std::endl;
            break;
        }
        default:
            std::cout << "whyyy" << std::endl;
    }
}
