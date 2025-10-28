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

            std::string tempName = makeTemporary();
            auto dst = std::make_unique<TackyIRVar>(tempName);

            auto tackyOp = generateTacky(unaryNode->op.get(), nullptr);

            instructions->instructions.push_back(
                std::make_unique<TackyIRUnary>(
                    std::move(tackyOp),
                    std::move(src),
                    std::move(dst)
                )
            );

            return std::make_unique<TackyIRVar>(tempName);
        }

        case NodeType::BINARY_OP: {
            const auto* binaryNode = static_cast<const BinaryNode*>(node);
            auto tackyOp = generateTacky(binaryNode->binaryOperator.get(), nullptr);
            auto v1 = generateTacky(binaryNode->expression1.get(), instructions);
            auto v2 = generateTacky(binaryNode->expression2.get(), instructions);
            
            std::string tempName = makeTemporary();
            auto dst = std::make_unique<TackyIRVar>(tempName);

            instructions->instructions.push_back(
                std::make_unique<TackyIRBinary>(
                    std::move(tackyOp),
                    std::move(v1),
                    std::move(v2),
                    std::move(dst)
                )
            );

            return std::make_unique<TackyIRVar>(tempName);
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

        case NodeType::ADD: {
            return std::make_unique<TackyIRAdd>();
        }

        case NodeType::SUBTRACT: {
            return std::make_unique<TackyIRSubtract>();
        }

        case NodeType::MULTIPLY: {
            return std::make_unique<TackyIRMultiply>();
        }

        case NodeType::DIVIDE: {
            return std::make_unique<TackyIRDivide>();
        }

        case NodeType::REMAINDER: {
            return std::make_unique<TackyIRRemainder>();
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
            std::cout << "body=[" << std::endl;
            for (const auto& instructionNode : functionNode->instructions->instructions) {
                printTacky(instructionNode.get(), count + 6);
            }
            printSpace(count + 3);
            std::cout << "]" << std::endl;
            printSpace(count);
            std::cout << ")" << std::endl;
            break;
        }
        case TackyIRNodeType::UNARY: {
            const TackyIRUnary* unaryNode = static_cast<const TackyIRUnary*>(node);
            printSpace(count);
            std::cout << "Unary(";
            printTacky(unaryNode->op.get(), 0);
            std::cout << ", ";
            printTacky(unaryNode->src.get(), 0);
            std::cout << ", ";
            printTacky(unaryNode->dst.get(), 0);
            std::cout << ")" << std::endl;
            break;
        }
        case TackyIRNodeType::BINARY: {
            const TackyIRBinary* binaryNode = static_cast<const TackyIRBinary*>(node);
            printSpace(count);
            std::cout << "Binary(";
            printTacky(binaryNode->op.get(), 0);
            std::cout << ", ";
            printTacky(binaryNode->src1.get(), 0);
            std::cout << ", ";
            printTacky(binaryNode->src2.get(), 0);
            std::cout << ", ";
            printTacky(binaryNode->dst.get(), 0);
            std::cout << ")" << std::endl;
            break;
        }
        case TackyIRNodeType::VAR: {
            const TackyIRVar* varNode = static_cast<const TackyIRVar*>(node);
            std::cout << "Var(\"" << varNode->value << "\")";
            break;
        }
        case TackyIRNodeType::NEGATE: {
            std::cout << "Negate";
            break;
        }
        case TackyIRNodeType::COMPLEMENT: {
            std::cout << "Complement";
            break;
        }
        case TackyIRNodeType::ADD: {
            std::cout << "Add";
            break;
        }
        case TackyIRNodeType::SUBTRACT: {
            std::cout << "Subtract";
            break;
        }
        case TackyIRNodeType::MULTIPLY: {
            std::cout << "Multiply";
            break;
        }
        case TackyIRNodeType::DIVIDE: {
            std::cout << "Divide";
            break;
        }
        case TackyIRNodeType::REMAINDER: {
            std::cout << "Remainder";
            break;
        }
        case TackyIRNodeType::RETURN: {
            const TackyIRReturn* returnNode = static_cast<const TackyIRReturn*>(node);
            printSpace(count);
            std::cout << "Return(";
            printTacky(returnNode->expr.get(), 0);
            std::cout << ")" << std::endl;
            break;
        }
        case TackyIRNodeType::CONSTANT: {
            const TackyIRConstant* constantNode = static_cast<const TackyIRConstant*>(node);
            std::cout << "Constant(" << constantNode->value << ")";
            break;
        }
        default:
            printSpace(count);
            std::cout << "SKIP" << std::endl;
            break;
    }
}
