#include "codegen.h"
#include "tacky_ir.h"
#include "ast.h"
#include <iostream>

std::unique_ptr<TackyIRNode> generateTacky(const Node* node) {
    if (!node) return nullptr;

    switch (node->type) {
        case NodeType::PROGRAM: {
            const auto* programNode = static_cast<const ProgramNode*>(node);
            auto func = generateTacky(programNode->function.get());

            return std::make_unique<TackyIRProgram>(std::unique_ptr<TackyIRFunction>(static_cast<TackyIRFunction*>(func.release()))
            );
        }

        case NodeType::FUNCTION: {
            const auto* functionNode = static_cast<const FunctionNode*>(node);
            auto instructions = generateTacky(functionNode->statement.get());

            return std::make_unique<TackyIRFunction>(
                functionNode->name,
                std::unique_ptr<TackyIRInstructions>(static_cast<TackyIRInstructions*>(instructions.release()))
            );
        }

        case NodeType::RETURN: {
            const auto* returnNode = static_cast<const ReturnNode*>(node);
            return nullptr;
        }

        case NodeType::CONSTANT: {
            const auto* constantNode = static_cast<const ConstantNode*>(node);
            return std::make_unique<TackyIRConstant>(constantNode->value);
        }

        default:
            return nullptr;
    }
}

void printTacky(const TackyIRNode* node, int count) {
    if (!node) return;

    switch (node->type) {
        case TackyIRNodeType::PROGRAM: {
            const TackyIRProgram* programNode = static_cast<const TackyIRProgram*>(node);
            std::cout << "Program(" << std::endl;
            printTacky(programNode->function.get(), count + 3);
            std::cout << ")" << std::endl;
            break;
        }
        case TackyIRNodeType::FUNCTION: {
            const TackyIRFunction* functionNode = static_cast<const TackyIRFunction*>(node);
            printSpace(count);
            std::cout << "Function(" << std::endl;
            printSpace(count + 3);
            std::cout << "name=" << functionNode->name << std::endl;
            printSpace(count + 3);
            std::cout << "body=";
            std::cout << "<SKIP>" << std::endl;
            //printTacky(functionNode->statement.get(), count + 3);
            printSpace(count);
            std::cout << ")" << std::endl;
            break;
        }
        case TackyIRNodeType::RETURN: {
            const TackyIRReturn* returnNode = static_cast<const TackyIRReturn*>(node);
            std::cout << "Return(" << std::endl;
            //printTacky(returnNode->expr.get(), count + 3);
            //printSpace(count);
            std::cout << ")" << std::endl;
            break;
        }
        case TackyIRNodeType::CONSTANT: {
            const TackyIRConstant* constantNode = static_cast<const TackyIRConstant*>(node);
            printSpace(count);
            std::cout << "Constant(" << constantNode->value << ")" << std::endl;
            break;
        }
    }
}