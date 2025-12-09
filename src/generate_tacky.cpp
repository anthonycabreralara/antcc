#include "tacky_ir.h"
#include "ast.h"
#include <iostream>

int temporaryAddress = 0;
int falseAndLabelCount = 0;
int trueOrLabelCount = 0;
int endLabelCount = 0;

std::string makeTemporary() {
    return "tmp" + std::to_string(temporaryAddress++) + ".o";
}

std::string makeFalseAndLabel() {
    return "and_false" + std::to_string(falseAndLabelCount++);
}

std::string makeTrueOrLabel() {
    return "or_true" + std::to_string(trueOrLabelCount++);
}
std::string makeEndLabel() {
    return "end" + std::to_string(endLabelCount++);
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
            //skip for parser
            //auto ret = generateTacky(functionNode->statement.get(), inst.get());
            //inst->instructions.push_back(std::move(ret));
            for (const auto& blockItem: functionNode->block->instructions) {
                auto item = generateTacky(blockItem.get(), inst.get());
            }

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

            if (binaryNode->binaryOperator->type == NodeType::AND) {
                std::string falseLabel = makeFalseAndLabel();
                std::string endLabel = makeEndLabel();
                std::string result = makeTemporary();

                auto v1 = generateTacky(binaryNode->expression1.get(), instructions);
                instructions->instructions.push_back(std::make_unique<TackyIRJumpIfZero>(std::move(v1), falseLabel));

                auto v2 = generateTacky(binaryNode->expression2.get(), instructions);
                instructions->instructions.push_back(std::make_unique<TackyIRJumpIfZero>(std::move(v2), falseLabel));

                instructions->instructions.push_back(std::make_unique<TackyIRCopy>(std::make_unique<TackyIRConstant>("1"), std::make_unique<TackyIRVar>(result)));

                instructions->instructions.push_back(std::make_unique<TackyIRJump>(endLabel));
                instructions->instructions.push_back(std::make_unique<TackyIRLabel>(falseLabel));
                instructions->instructions.push_back(std::make_unique<TackyIRCopy>(std::make_unique<TackyIRConstant>("0"), std::make_unique<TackyIRVar>(result)));
                instructions->instructions.push_back(std::make_unique<TackyIRLabel>(endLabel));

                return std::make_unique<TackyIRVar>(result);
            } else if (binaryNode->binaryOperator->type == NodeType::OR) {
                std::string trueLabel = makeTrueOrLabel();
                std::string endLabel = makeEndLabel();
                std::string result = makeTemporary();

                auto v1 = generateTacky(binaryNode->expression1.get(), instructions);
                instructions->instructions.push_back(std::make_unique<TackyIRJumpIfNotZero>(std::move(v1), trueLabel));

                auto v2 = generateTacky(binaryNode->expression2.get(), instructions);
                instructions->instructions.push_back(std::make_unique<TackyIRJumpIfNotZero>(std::move(v2), trueLabel));

                instructions->instructions.push_back(std::make_unique<TackyIRCopy>(std::make_unique<TackyIRConstant>("0"), std::make_unique<TackyIRVar>(result)));

                instructions->instructions.push_back(std::make_unique<TackyIRJump>(endLabel));
                instructions->instructions.push_back(std::make_unique<TackyIRLabel>(trueLabel));
                instructions->instructions.push_back(std::make_unique<TackyIRCopy>(std::make_unique<TackyIRConstant>("1"), std::make_unique<TackyIRVar>(result)));
                instructions->instructions.push_back(std::make_unique<TackyIRLabel>(endLabel));

                return std::make_unique<TackyIRVar>(result);
            } else {
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

        case NodeType::NOT: {
            return std::make_unique<TackyIRNot>();
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

        case NodeType::EQUAL: {
            return std::make_unique<TackyIREqual>();
        }

        case NodeType::NOT_EQUAL: {
            return std::make_unique<TackyIRNotEqual>();
        }

        case NodeType::LESS_THAN: {
            return std::make_unique<TackyIRLessThan>();
        }

        case NodeType::LESS_OR_EQUAL: {
            return std::make_unique<TackyIRLessOrEqual>();
        }

        case NodeType::GREATER_THAN: {
            return std::make_unique<TackyIRGreaterThan>();
        }

        case NodeType::GREATER_OR_EQUAL: {
            return std::make_unique<TackyIRGreaterOrEqual>();
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
        case TackyIRNodeType::NOT: {
            std::cout << "Not";
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
        case TackyIRNodeType::EQUAL: {
            std::cout << "Equal";
            break;
        }
        case TackyIRNodeType::NOT_EQUAL: {
            std::cout << "NotEqual";
            break;
        }
        case TackyIRNodeType::LESS_THAN: {
            std::cout << "LessThan";
            break;
        }
        case TackyIRNodeType::LESS_OR_EQUAL: {
            std::cout << "LessOrEqual";
            break;
        }
        case TackyIRNodeType::GREATER_THAN: {
            std::cout << "GreaterThan";
            break;
        }
        case TackyIRNodeType::GREATER_OR_EQUAL: {
            std::cout << "GreaterOrEqual";
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
        case TackyIRNodeType::JUMP: {
            printSpace(count);
            const TackyIRJump* jumpNode = static_cast<const TackyIRJump*>(node);
            std::cout << "Jump(" << jumpNode->target << ")" << std::endl;
            break;
        }
        case TackyIRNodeType::JUMP_IF_ZERO: {
            const TackyIRJumpIfZero* jumpIfZeroNode = static_cast<const TackyIRJumpIfZero*>(node);
            printSpace(count);
            std::cout << "JumpIfZero(";
            printTacky(jumpIfZeroNode->condition.get(), 0);
            std::cout << ", " << jumpIfZeroNode->target << ")" << std::endl;
            break;
        }
        case TackyIRNodeType::JUMP_IF_NOT_ZERO: {
            const TackyIRJumpIfNotZero* jumpIfNotZero = static_cast<const TackyIRJumpIfNotZero*>(node);
            printSpace(count);
            std::cout << "JumpIfNotZero(";
            printTacky(jumpIfNotZero->condition.get(), 0);
            std::cout << ", " << jumpIfNotZero->target << ")" << std::endl;
            break;
        }
        case TackyIRNodeType::COPY: {
            const TackyIRCopy* copyNode = static_cast<const TackyIRCopy*>(node);
            printSpace(count);
            std::cout << "Copy(";
            printTacky(copyNode->src.get(), 0);
            std::cout << ", ";
            printTacky(copyNode->dst.get(), 0);
            std::cout << ")" << std::endl;
            break;
        }
        case TackyIRNodeType::LABEL: {
            const TackyIRLabel* labelNode = static_cast<const TackyIRLabel*>(node);
            printSpace(count);
            std::cout << "Label(" << labelNode->identifier << ")" << std::endl;
            break;
        }

        default:
            printSpace(count);
            std::cout << "SKIP" << std::endl;
            break;
    }
}
