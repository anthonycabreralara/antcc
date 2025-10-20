#include "codegen.h"
#include "asm_ir.h"
#include "tacky_ir.h"
#include <iostream>

// --- Flatten nested AsmIRInstructions ---
std::unique_ptr<AsmIRInstructions> unnestInstructions(std::unique_ptr<AsmIRInstructions> node) {
    auto flat = std::make_unique<AsmIRInstructions>();
    if (!node) return flat;

    for (auto& instr : node->instructions) {
        if (!instr) continue;

        if (instr->type == AsmIRNodeType::INSTRUCTIONS) {
            auto nested = static_cast<AsmIRInstructions*>(instr.release());
            auto nestedFlat = unnestInstructions(std::unique_ptr<AsmIRInstructions>(nested));
            for (auto& nestedInstr : nestedFlat->instructions)
                flat->instructions.push_back(std::move(nestedInstr));
        } else {
            flat->instructions.push_back(std::move(instr));
        }
    }

    return flat;
}

// --- Generate Asm IR ---
std::unique_ptr<AsmIRNode> generateCode(const TackyIRNode* node, AsmIRInstructions* instructions) {
    if (!node) return nullptr;

    switch (node->type) {
        case TackyIRNodeType::PROGRAM: {
            const auto* programNode = static_cast<const TackyIRProgram*>(node);
            auto func = generateCode(programNode->function.get(), nullptr);
            return std::make_unique<AsmIRProgram>(
                std::unique_ptr<AsmIRFunction>(static_cast<AsmIRFunction*>(func.release()))
            );
        }

        case TackyIRNodeType::FUNCTION: {
            const auto* functionNode = static_cast<const TackyIRFunction*>(node);
            auto asmFunctionInstructions = std::make_unique<AsmIRInstructions>();

            for (const auto& instructionNode : functionNode->instructions->instructions) {
                generateCode(instructionNode.get(), asmFunctionInstructions.get());
            }

            auto flattened = unnestInstructions(std::move(asmFunctionInstructions));
            return std::make_unique<AsmIRFunction>(functionNode->name, std::move(flattened));
        }

        case TackyIRNodeType::RETURN: {
            const auto* returnNode = static_cast<const TackyIRReturn*>(node);
            auto expr = generateCode(returnNode->expr.get(), nullptr);
            auto dst = std::make_unique<AsmIRReg>("eax");

            if (instructions) {
                instructions->instructions.push_back(std::make_unique<AsmIRMov>(std::move(expr), std::move(dst)));
                instructions->instructions.push_back(std::make_unique<AsmIRRet>());
            }

            return nullptr;
        }

        case TackyIRNodeType::UNARY: {
            const auto* unaryNode = static_cast<const TackyIRUnary*>(node);
            auto unaryOperator = generateCode(unaryNode->op.get(), nullptr);
            auto src = generateCode(unaryNode->src.get(), nullptr);
            auto dst_1 = generateCode(unaryNode->dst.get(), nullptr);
            auto dst_2 = generateCode(unaryNode->dst.get(), nullptr);

            if (instructions) {
                instructions->instructions.push_back(std::make_unique<AsmIRMov>(std::move(src), std::move(dst_1)));
                instructions->instructions.push_back(std::make_unique<AsmIRUnary>(std::move(unaryOperator), std::move(dst_2)));
            }

            return nullptr;
        }

        case TackyIRNodeType::NEGATE: return std::make_unique<AsmIRNeg>();
        case TackyIRNodeType::COMPLEMENT: return std::make_unique<AsmIRNot>();
        case TackyIRNodeType::CONSTANT: {
            const auto* constantNode = static_cast<const TackyIRConstant*>(node);
            return std::make_unique<AsmIRImm>(constantNode->value);
        }
        case TackyIRNodeType::VAR: {
            const auto* varNode = static_cast<const TackyIRVar*>(node);
            return std::make_unique<AsmIRPseudo>(varNode->value);
        }

        default:
            return nullptr;
    }
}

// --- Pretty IR Printer ---
void printIR(const AsmIRNode* node, int space = 0) {
    if (!node) return;

    auto indent = std::string(space, ' ');

    switch (node->type) {
        case AsmIRNodeType::PROGRAM: {
            std::cout << indent << "Program(\n";
            const auto* programNode = static_cast<const AsmIRProgram*>(node);
            printIR(programNode->function.get(), space + 2);
            std::cout << indent << ")\n";
            break;
        }
        case AsmIRNodeType::FUNCTION: {
            const auto* fn = static_cast<const AsmIRFunction*>(node);
            std::cout << indent << "Function(name=" << fn->name << ", body=[\n";
            if (fn->instructions) {
                for (const auto& instr : fn->instructions->instructions)
                    printIR(instr.get(), space + 4);
            }
            std::cout << indent << "])\n";
            break;
        }
        case AsmIRNodeType::MOV: {
            const auto* moveNode = static_cast<const AsmIRMov*>(node);
            std::cout << indent << "Mov(";
            printIR(moveNode->src.get(), 0);
            std::cout << ", ";
            printIR(moveNode->dst.get(), 0);
            std::cout << ")\n";
            break;
        }
        case AsmIRNodeType::IMMEDIATE: {
            const auto* immNode = static_cast<const AsmIRImm*>(node);
            std::cout << "Imm(" << immNode->value << ")";
            break;
        }
        case AsmIRNodeType::PSEUDO: {
            std::cout << "Pseudo()";
            break;
        }
        case AsmIRNodeType::REGISTER: {
            const auto* regNode = static_cast<const AsmIRReg*>(node);
            std::cout << "Register(" << regNode->value << ")";
            break;
        }
        case AsmIRNodeType::RETURN:
            std::cout << indent << "Return()\n";
            break;
        case AsmIRNodeType::UNARY:
            std::cout << indent << "Unary()\n";
            break;
        default:
            std::cout << indent << "UnknownNode(type=" << static_cast<int>(node->type) << ")\n";
            break;
    }
}