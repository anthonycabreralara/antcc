#include "codegen.h"
#include "tacky_ir.h"
#include "ast.h"
#include <iostream>

int temporaryAddress = 0;

std::string makeTemporary() {
    return "tmp" + std::to_string(temporaryAddress++) + ".o";
}

std::unique_ptr<TackyIRNode> generateTacky(const Node* node, TackyIRInstructions* instructions) {
    if (!node) return nullptr;

    switch (node->type) {
        case NodeType::PROGRAM: {
            const auto* programNode = static_cast<const ProgramNode*>(node);
            auto funcNode = generateTacky(programNode->function.get(), nullptr);
            return std::make_unique<TackyIRProgram>(
                std::unique_ptr<TackyIRFunction>(static_cast<TackyIRFunction*>(funcNode.release()))
            );
        }

        case NodeType::FUNCTION: {
            const auto* functionNode = static_cast<const FunctionNode*>(node);
            auto inst = std::make_unique<TackyIRInstructions>();
            auto ret = generateTacky(functionNode->statement.get(), inst.get());
            inst->instructions.push_back(std::move(ret));

            return std::make_unique<TackyIRFunction>(
                functionNode->name,
                std::unique_ptr<TackyIRInstructions>(static_cast<TackyIRInstructions*>(inst.release()))
            );
        }

        case NodeType::RETURN: {
            const auto* returnNode = static_cast<const ReturnNode*>(node);
            auto ret = generateTacky(returnNode->expr.get(), instructions);
            return std::make_unique<TackyIRReturn>(std::move(ret));
        }

        case NodeType::UNARY_OP: {
            const auto* unaryNode = static_cast<const UnOpNode*>(node);
            auto src = generateTacky(unaryNode->expr.get(), instructions);
            auto dstName = makeTemporary();
            auto dst = std::make_unique<TackyIRVar>(dstName);
            auto tackyOp = generateTacky(unaryNode->op.get(), nullptr);
            instructions->instructions.push_back(std::make_unique<TackyIRUnary>(std::move(tackyOp), std::move(src), std::move(dst)));
            return std::unique_ptr<TackyIRNode>(std::move(dst));
        }

        case NodeType::CONSTANT: {
            const auto* constantNode = static_cast<const ConstantNode*>(node);
            return std::make_unique<TackyIRConstant>(constantNode->value);
        }

        case NodeType::NEGATE: {
            return std::make_unique<TackyIRNegate>();
        }

        case NodeType::COMPLEMENT: {
            return std::make_unique<TackyIRComplement>();
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