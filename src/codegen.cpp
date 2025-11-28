#include "codegen.h"
#include "asm_ir.h"
#include "tacky_ir.h"
#include <iostream>
#include <memory>
#include <unordered_map>

// --- Flatten nested AsmIRInstructions ---
std::unique_ptr<AsmIRInstructions> passUnnest(std::unique_ptr<AsmIRInstructions> node) {
    auto flat = std::make_unique<AsmIRInstructions>();
    if (!node) return flat;

    for (auto& instr : node->instructions) {
        if (!instr) continue;

        if (instr->type == AsmIRNodeType::INSTRUCTIONS) {
            auto nested = static_cast<AsmIRInstructions*>(instr.release());
            auto nestedFlat = passUnnest(std::unique_ptr<AsmIRInstructions>(nested));
            for (auto& nestedInstr : nestedFlat->instructions)
                flat->instructions.push_back(std::move(nestedInstr));
        } else {
            flat->instructions.push_back(std::move(instr));
        }
    }

    return flat;
}

// --- 1st Asm IR Pass---
std::unique_ptr<AsmIRNode> buildAsmIRAst(const TackyIRNode* node, AsmIRInstructions* instructions) {
    if (!node) return nullptr;

    switch (node->type) {
        case TackyIRNodeType::PROGRAM: {
            const auto* programNode = static_cast<const TackyIRProgram*>(node);
            auto func = buildAsmIRAst(programNode->function.get(), nullptr);
            return std::make_unique<AsmIRProgram>(
                std::unique_ptr<AsmIRFunction>(static_cast<AsmIRFunction*>(func.release()))
            );
        }

        case TackyIRNodeType::FUNCTION: {
            const auto* functionNode = static_cast<const TackyIRFunction*>(node);
            auto asmFunctionInstructions = std::make_unique<AsmIRInstructions>();

            for (const auto& instructionNode : functionNode->instructions->instructions) {
                buildAsmIRAst(instructionNode.get(), asmFunctionInstructions.get());
            }

            auto flattened = passUnnest(std::move(asmFunctionInstructions));
            return std::make_unique<AsmIRFunction>(functionNode->name, std::move(flattened));
        }

        case TackyIRNodeType::RETURN: {
            const auto* returnNode = static_cast<const TackyIRReturn*>(node);
            auto expr = buildAsmIRAst(returnNode->expr.get(), nullptr);
            auto dst = std::make_unique<AsmIRReg>("AX");

            if (instructions) {
                instructions->instructions.push_back(std::make_unique<AsmIRMov>(std::move(expr), std::move(dst)));
                instructions->instructions.push_back(std::make_unique<AsmIRRet>());
            }

            return nullptr;
        }

        case TackyIRNodeType::UNARY: {
            const auto* unaryNode = static_cast<const TackyIRUnary*>(node);
            auto unaryOperator = buildAsmIRAst(unaryNode->op.get(), nullptr);
            auto src = buildAsmIRAst(unaryNode->src.get(), nullptr);
            auto dst_1 = buildAsmIRAst(unaryNode->dst.get(), nullptr);
            auto dst_2 = buildAsmIRAst(unaryNode->dst.get(), nullptr);

            if (unaryNode->op->type == TackyIRNodeType::NOT && instructions) {
                instructions->instructions.push_back(std::make_unique<AsmIRCmp>(
                    std::make_unique<AsmIRImm>("0"),
                    std::move(src)
                ));
                instructions->instructions.push_back(std::make_unique<AsmIRMov>(
                    std::make_unique<AsmIRImm>("0"),
                    std::move(dst_1)
                ));
                instructions->instructions.push_back(std::make_unique<AsmIRSetCC>(
                    "E",
                    std::move(dst_2)
                ));
            } else if (instructions) {
                instructions->instructions.push_back(std::make_unique<AsmIRMov>(std::move(src), std::move(dst_1)));
                instructions->instructions.push_back(std::make_unique<AsmIRUnary>(std::move(unaryOperator), std::move(dst_2)));
            }

            return nullptr;
        }
        case TackyIRNodeType::BINARY: {
            const auto* binaryNode = static_cast<const TackyIRBinary*>(node);


            if (binaryNode->op->type == TackyIRNodeType::DIVIDE) {
                /*
                    Mov(src1, Reg(AX))
                    Cdq
                    Idiv(src2)
                    Mov(Reg(AX), dst)
                */
                auto src1 = buildAsmIRAst(binaryNode->src1.get(), nullptr);
                auto src2 = buildAsmIRAst(binaryNode->src2.get(), nullptr);
                auto dst = buildAsmIRAst(binaryNode->dst.get(), nullptr);

                if (instructions) {
                    instructions->instructions.push_back(std::make_unique<AsmIRMov>(std::move(src1), std::make_unique<AsmIRReg>("AX")));
                    instructions->instructions.push_back(std::make_unique<AsmIRCdq>());
                    instructions->instructions.push_back(std::make_unique<AsmIRIdiv>(std::move(src2)));
                    instructions->instructions.push_back(std::make_unique<AsmIRMov>(std::make_unique<AsmIRReg>("AX"), std::move(dst)));
                }
            } else if (binaryNode->op->type == TackyIRNodeType::REMAINDER) {
                /*
                    Mov(src1, Reg(AX))
                    Cdq
                    Idiv(src2)
                    Mov(Reg(DX), dst)
                */
                auto src1 = buildAsmIRAst(binaryNode->src1.get(), nullptr);
                auto src2 = buildAsmIRAst(binaryNode->src2.get(), nullptr);
                auto dst = buildAsmIRAst(binaryNode->dst.get(), nullptr);

                if (instructions) {
                    instructions->instructions.push_back(std::make_unique<AsmIRMov>(std::move(src1), std::make_unique<AsmIRReg>("AX")));
                    instructions->instructions.push_back(std::make_unique<AsmIRCdq>());
                    instructions->instructions.push_back(std::make_unique<AsmIRIdiv>(std::move(src2)));
                    instructions->instructions.push_back(std::make_unique<AsmIRMov>(std::make_unique<AsmIRReg>("DX"), std::move(dst)));
                }
            } else if (binaryNode->op->type == TackyIRNodeType::EQUAL ||
                        binaryNode->op->type == TackyIRNodeType::NOT_EQUAL ||
                        binaryNode->op->type == TackyIRNodeType::LESS_THAN ||
                        binaryNode->op->type == TackyIRNodeType::GREATER_THAN ||
                        binaryNode->op->type == TackyIRNodeType::LESS_OR_EQUAL ||
                        binaryNode->op->type == TackyIRNodeType::GREATER_OR_EQUAL) {

                std::string cond_code = "";

                if (binaryNode->op->type == TackyIRNodeType::EQUAL) {
                    cond_code = "E";
                } else if (binaryNode->op->type == TackyIRNodeType::NOT_EQUAL) {
                    cond_code = "NE";
                } else if (binaryNode->op->type == TackyIRNodeType::LESS_THAN) {
                    cond_code = "L";
                } else if (binaryNode->op->type == TackyIRNodeType::GREATER_THAN) {
                    cond_code = "G";
                } else if (binaryNode->op->type == TackyIRNodeType::LESS_OR_EQUAL) {
                    cond_code = "LE";
                } else if (binaryNode->op->type == TackyIRNodeType::GREATER_OR_EQUAL) {
                    cond_code = "GE";
                }

                auto src1 = buildAsmIRAst(binaryNode->src1.get(), nullptr);
                auto src2 = buildAsmIRAst(binaryNode->src2.get(), nullptr);
                auto dst_1 = buildAsmIRAst(binaryNode->dst.get(), nullptr);
                auto dst_2 = buildAsmIRAst(binaryNode->dst.get(), nullptr);

                instructions->instructions.push_back(std::make_unique<AsmIRCmp>(
                    std::move(src1),
                    std::move(src2)
                ));
                instructions->instructions.push_back(std::make_unique<AsmIRMov>(
                    std::make_unique<AsmIRImm>("0"),
                    std::move(dst_1)
                ));
                instructions->instructions.push_back(std::make_unique<AsmIRSetCC>(
                    cond_code,
                    std::move(dst_2)
                ));
                return nullptr;
            } else {
                /*
                    Mov(src1, dst)
                    Binary(binary_operator, src2, dst)
                */
                auto binaryOperator = buildAsmIRAst(binaryNode->op.get(), nullptr);
                auto src1 = buildAsmIRAst(binaryNode->src1.get(), nullptr);
                auto src2 = buildAsmIRAst(binaryNode->src2.get(), nullptr);
                auto dst_1 = buildAsmIRAst(binaryNode->dst.get(), nullptr);
                auto dst_2 = buildAsmIRAst(binaryNode->dst.get(), nullptr);

                if (instructions) {
                    instructions->instructions.push_back(std::make_unique<AsmIRMov>(std::move(src1), std::move(dst_1)));
                    instructions->instructions.push_back(std::make_unique<AsmIRBinary>(std::move(binaryOperator), std::move(src2), std::move(dst_2)));
                }
            }

            return nullptr;
        }
        case TackyIRNodeType::JUMP: {
            const auto* jumpNode = static_cast<const TackyIRJump*>(node);
            return std::make_unique<AsmIRJmp>(jumpNode->target);
        }
        case TackyIRNodeType::JUMP_IF_ZERO: {
            const auto* jumpNode = static_cast<const TackyIRJumpIfZero*>(node);
            auto condition = buildAsmIRAst(jumpNode->condition.get(), nullptr);
            instructions->instructions.push_back(std::make_unique<AsmIRCmp>(
                std::make_unique<AsmIRImm>("0"),
                std::move(condition)
            ));
            instructions->instructions.push_back(std::make_unique<AsmIRJmpCC>(
                "E",
                jumpNode->target
            ));
            return nullptr;
        }
        case TackyIRNodeType::JUMP_IF_NOT_ZERO: {
            const auto* jumpNode = static_cast<const TackyIRJumpIfNotZero*>(node);
            auto condition = buildAsmIRAst(jumpNode->condition.get(), nullptr);
            instructions->instructions.push_back(std::make_unique<AsmIRCmp>(
                std::make_unique<AsmIRImm>("0"),
                std::move(condition)
            ));
            instructions->instructions.push_back(std::make_unique<AsmIRJmpCC>(
                "NE",
                jumpNode->target
            ));
            return nullptr;
        }
        case TackyIRNodeType::COPY: {
            const auto* copyNode = static_cast<const TackyIRCopy*>(node);
            auto src = buildAsmIRAst(copyNode->src.get(), nullptr);
            auto dst = buildAsmIRAst(copyNode->dst.get(), nullptr);
            instructions->instructions.push_back(std::make_unique<AsmIRMov>(
                std::move(src),
                std::move(dst)
            ));
            return nullptr;
        }
        case TackyIRNodeType::LABEL: {
            const auto* labelNode = static_cast<const TackyIRLabel*>(node);
            instructions->instructions.push_back(std::make_unique<AsmIRLabel>(labelNode->identifier));
            return nullptr;
        }
        case TackyIRNodeType::NEGATE: return std::make_unique<AsmIRNeg>();
        case TackyIRNodeType::COMPLEMENT: return std::make_unique<AsmIRNot>();
        case TackyIRNodeType::ADD: return std::make_unique<AsmIRAdd>();
        case TackyIRNodeType::SUBTRACT: return std::make_unique<AsmIRSubtract>();
        case TackyIRNodeType::MULTIPLY: return std::make_unique<AsmIRMultiply>();
        case TackyIRNodeType::CONSTANT: {
            const auto* constantNode = static_cast<const TackyIRConstant*>(node);
            return std::make_unique<AsmIRImm>(constantNode->value);
        }
        case TackyIRNodeType::VAR: {
            const auto* varNode = static_cast<const TackyIRVar*>(node);
            return std::make_unique<AsmIRPseudo>(varNode->value);
        }

        default:
            std::cout << "Error: TackyIR -> AsmIR" << std::endl;
            return nullptr;
    }
}

void passReplacePseudos(AsmIRNode* node, std::unordered_map<std::string, int>& pseudoToOffset, int& nextOffset) {
    if (!node) return;

    switch (node->type) {
        case AsmIRNodeType::PROGRAM: {
            auto* programNode = static_cast<AsmIRProgram*>(node);
            passReplacePseudos(programNode->function.get(), pseudoToOffset, nextOffset);
            break;
        }

        case AsmIRNodeType::FUNCTION: {
            auto* fn = static_cast<AsmIRFunction*>(node);
            if (fn->instructions) {
                for (auto& instr : fn->instructions->instructions) {
                    passReplacePseudos(instr.get(), pseudoToOffset, nextOffset);
                }
            }
            break;
        }

        case AsmIRNodeType::MOV: {
            auto* move = static_cast<AsmIRMov*>(node);

            // source
            if (move->src->type == AsmIRNodeType::PSEUDO) {
                auto* pseudo = static_cast<AsmIRPseudo*>(move->src.get());
                auto& id = pseudo->identifier;
                if (!pseudoToOffset.count(id)) {
                    pseudoToOffset[id] = nextOffset;
                    nextOffset -= 4;
                }
                move->src = std::make_unique<AsmIRStack>(pseudoToOffset[id]);
            }

            // destination
            if (move->dst->type == AsmIRNodeType::PSEUDO) {
                auto* pseudo = static_cast<AsmIRPseudo*>(move->dst.get());
                auto& id = pseudo->identifier;
                if (!pseudoToOffset.count(id)) {
                    pseudoToOffset[id] = nextOffset;
                    nextOffset -= 4;
                }
                move->dst = std::make_unique<AsmIRStack>(pseudoToOffset[id]);
            }
            break;
        }

        case AsmIRNodeType::UNARY: {
            auto* unary = static_cast<AsmIRUnary*>(node);
            if (unary->operand->type == AsmIRNodeType::PSEUDO) {
                auto* pseudo = static_cast<AsmIRPseudo*>(unary->operand.get());
                auto& id = pseudo->identifier;
                if (!pseudoToOffset.count(id)) {
                    pseudoToOffset[id] = nextOffset;
                    nextOffset -= 4;
                }
                unary->operand = std::make_unique<AsmIRStack>(pseudoToOffset[id]);
            }
            break;
        }

        case AsmIRNodeType::BINARY: {
            auto* binary = static_cast<AsmIRBinary*>(node);
            if (binary->operand1->type == AsmIRNodeType::PSEUDO) {
                auto* pseudo = static_cast<AsmIRPseudo*>(binary->operand1.get());
                auto& id = pseudo->identifier;
                if (!pseudoToOffset.count(id)) {
                    pseudoToOffset[id] = nextOffset;
                    nextOffset -= 4;
                }
                binary->operand1 = std::make_unique<AsmIRStack>(pseudoToOffset[id]);
            }

            if (binary->operand2->type == AsmIRNodeType::PSEUDO) {
                auto* pseudo = static_cast<AsmIRPseudo*>(binary->operand2.get());
                auto& id = pseudo->identifier;
                if (!pseudoToOffset.count(id)) {
                    pseudoToOffset[id] = nextOffset;
                    nextOffset -= 4;
                }
                binary->operand2 = std::make_unique<AsmIRStack>(pseudoToOffset[id]);
            }
            break;
        }

        case AsmIRNodeType::IDIV: {
            auto* idiv = static_cast<AsmIRIdiv*>(node);
            if (idiv->operand->type == AsmIRNodeType::PSEUDO) {
                auto* pseudo = static_cast<AsmIRPseudo*>(idiv->operand.get());
                auto& id = pseudo->identifier;
                if (!pseudoToOffset.count(id)) {
                    pseudoToOffset[id] = nextOffset;
                    nextOffset -= 4;
                }
                idiv->operand = std::make_unique<AsmIRStack>(pseudoToOffset[id]);
            }
            break;
        }
        
        case AsmIRNodeType::SET_CC: {
            auto* setCC = static_cast<AsmIRSetCC*>(node);
            if (setCC->operand->type == AsmIRNodeType::PSEUDO) {
                auto* pseudo = static_cast<AsmIRPseudo*>(setCC->operand.get());
                auto& id = pseudo->identifier;
                if (!pseudoToOffset.count(id)) {
                    pseudoToOffset[id] = nextOffset;
                    nextOffset -= 4;
                }
                setCC->operand = std::make_unique<AsmIRStack>(pseudoToOffset[id]);
            }
            break;
        }


        default:
            break;
    }
}

std::unique_ptr<AsmIRNode> passFixes(std::unique_ptr<AsmIRNode> node, AsmIRInstructions* instructions, int& nextOffset) {
    if (!node) return nullptr;

    switch (node->type) {
        case AsmIRNodeType::PROGRAM: {
            auto* programNode = static_cast<AsmIRProgram*>(node.get());
            auto newFn = passFixes(std::move(programNode->function), nullptr, nextOffset);
            programNode->function = std::unique_ptr<AsmIRFunction>(
                static_cast<AsmIRFunction*>(newFn.release())
            );
            return node;
        }

        case AsmIRNodeType::FUNCTION: {
            auto* fn = static_cast<AsmIRFunction*>(node.get());
            auto asmFunctionInstructions = std::make_unique<AsmIRInstructions>();

            if (fn->instructions) {
                auto &oldVec = fn->instructions->instructions;
                std::vector<std::unique_ptr<AsmIRNode>> newVec;
                newVec.reserve(oldVec.size());

                asmFunctionInstructions->instructions.push_back(
                    std::make_unique<AsmIRAllocateStack>(-(nextOffset + 4))
                );

                for (auto &oldInstr : oldVec) {
                    std::unique_ptr<AsmIRNode> taken = std::move(oldInstr);
                    passFixes(std::move(taken), asmFunctionInstructions.get(), nextOffset);
                }

                fn->instructions = std::move(asmFunctionInstructions);
                for (auto &n : newVec)
                    fn->instructions->instructions.push_back(std::move(n));
            }

            return node;
        }

        case AsmIRNodeType::MOV: {
            auto* move = static_cast<AsmIRMov*>(node.get());

            if (move->src->type == AsmIRNodeType::STACK &&
                move->dst->type == AsmIRNodeType::STACK) {
                auto src = std::move(move->src);
                auto dst = std::move(move->dst);
                instructions->instructions.push_back(std::make_unique<AsmIRMov>(std::move(src), std::make_unique<AsmIRReg>("R10")));
                instructions->instructions.push_back(std::make_unique<AsmIRMov>(std::make_unique<AsmIRReg>("R10"), std::move(dst)));
                return nullptr;
            } else {
                if (instructions) {
                    instructions->instructions.push_back(std::move(node));
                    return nullptr;
                } else {
                    return node;
                }
            }
        }

        case AsmIRNodeType::UNARY: {
            if (instructions) {
                instructions->instructions.push_back(std::move(node));
                return nullptr;
            }
            return node;
        }

        case AsmIRNodeType::BINARY: {
            auto* binary = static_cast<AsmIRBinary*>(node.get());
            if (instructions &&
                (binary->binary_operator->type == AsmIRNodeType::ADD || binary->binary_operator->type == AsmIRNodeType::SUBTRACT) &&
                (binary->operand1->type == AsmIRNodeType::STACK && binary->operand2->type == AsmIRNodeType::STACK)) {

                auto binary_operator = std::move(binary->binary_operator);
                auto operand1 = std::move(binary->operand1);
                auto operand2 = std::move(binary->operand2);
                instructions->instructions.push_back(std::make_unique<AsmIRMov>(std::move(operand1), std::make_unique<AsmIRReg>("R10")));
                instructions->instructions.push_back(std::make_unique<AsmIRBinary>(std::move(binary_operator), std::make_unique<AsmIRReg>("R10"), std::move(operand2)));

            } else if (instructions &&
                binary->binary_operator->type == AsmIRNodeType::MULTIPLY &&
                binary->operand2->type == AsmIRNodeType::STACK) {

                auto binary_operator = std::move(binary->binary_operator);
                auto operand1 = std::move(binary->operand1);

                auto* casted_operand2 = dynamic_cast<AsmIRStack*>(binary->operand2.get());
                auto operand2_2 = std::make_unique<AsmIRStack>(casted_operand2->stack_size);
                auto operand2 = std::move(binary->operand2);

                instructions->instructions.push_back(std::make_unique<AsmIRMov>(std::move(operand2), std::make_unique<AsmIRReg>("R11")));
                instructions->instructions.push_back(std::make_unique<AsmIRBinary>(std::move(binary_operator), std::move(operand1), std::make_unique<AsmIRReg>("R11")));
                instructions->instructions.push_back(std::make_unique<AsmIRMov>(std::make_unique<AsmIRReg>("R11"), std::move(operand2_2)));
            } else if (instructions) {
                instructions->instructions.push_back(std::move(node));
                return nullptr;
            }
            return nullptr;
        }

        case AsmIRNodeType::CDQ: {
            if (instructions) {
                instructions->instructions.push_back(std::move(node));
                return nullptr;
            }
            return node;
        }

        case AsmIRNodeType::CMP: {
            if (instructions) {
                instructions->instructions.push_back(std::move(node));
                return nullptr;
            }
            return node;
        }

        case AsmIRNodeType::SET_CC: {
            if (instructions) {
                instructions->instructions.push_back(std::move(node));
                return nullptr;
            }
            return node;
        }

        case AsmIRNodeType::JMP: {
            if (instructions) {
                instructions->instructions.push_back(std::move(node));
                return nullptr;
            }
            return node;
        }

        case AsmIRNodeType::JMP_CC: {
            if (instructions) {
                instructions->instructions.push_back(std::move(node));
                return nullptr;
            }
            return node;
        }

        case AsmIRNodeType::LABEL: {
            if (instructions) {
                instructions->instructions.push_back(std::move(node));
                return nullptr;
            }
            return node;
        }
        
        case AsmIRNodeType::IDIV: {
            auto* idiv = static_cast<AsmIRIdiv*>(node.get());
            if (idiv->operand->type == AsmIRNodeType::IMMEDIATE) {
                auto operand = std::move(idiv->operand);
                instructions->instructions.push_back(std::make_unique<AsmIRMov>(std::move(operand), std::make_unique<AsmIRReg>("R10")));
                instructions->instructions.push_back(std::make_unique<AsmIRIdiv>(std::make_unique<AsmIRReg>("R10")));
                return nullptr;
            } else {
                if (instructions) {
                    instructions->instructions.push_back(std::move(node));
                    return nullptr;
                }

                return node;
            }
        }

        case AsmIRNodeType::RETURN: {
            if (instructions) {
                instructions->instructions.push_back(std::move(node));
                return nullptr;
            }
            return node;
        }

        default:
            std::cout << "Error: AsmIRNode not handled through pass." << std::endl;
            return node;
    }
}


// --- Generate Asm IR ---
std::unique_ptr<AsmIRNode> generateCode(const TackyIRNode* node) {
    auto asm_ir = buildAsmIRAst(node, nullptr);
    std::unordered_map<std::string, int> pseudoToOffset;
    int nextOffset = -4;

    passReplacePseudos(asm_ir.get(), pseudoToOffset, nextOffset);
    asm_ir = passFixes(std::move(asm_ir), nullptr, nextOffset);
    return std::move(asm_ir);
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
            const auto* pseudoNode = static_cast<const AsmIRPseudo*>(node);
            std::cout << "Pseudo(" << pseudoNode->identifier << ")";
            break;
        }
        case AsmIRNodeType::REGISTER: {
            const auto* regNode = static_cast<const AsmIRReg*>(node);
            std::cout << "Register(" << regNode->value << ")";
            break;
        }
        case AsmIRNodeType::RETURN: {
            std::cout << indent << "Return()" << std::endl;
            break;
        }
        case AsmIRNodeType::UNARY: {
            const auto* unaryNode = static_cast<const AsmIRUnary*>(node);
            std::cout << indent << "Unary(";
            printIR(unaryNode->unary_operator.get(), 0);
            std::cout << ", ";
            printIR(unaryNode->operand.get(), 0);
            std::cout << ")\n";
            break;
        }
        case AsmIRNodeType::BINARY: {
            const auto* binaryNode = static_cast<const AsmIRBinary*>(node);
            std::cout << indent << "Binary(";
            printIR(binaryNode->binary_operator.get(), 0);
            std::cout << ", ";
            printIR(binaryNode->operand1.get(), 0);
            std::cout << ", ";
            printIR(binaryNode->operand2.get(), 0);
            std::cout << ")" << std::endl;
            break;
        }
        case AsmIRNodeType::IDIV: {
            const auto* idivNode = static_cast<const AsmIRIdiv*>(node);
            std::cout << indent << "Idiv(";
            printIR(idivNode->operand.get(), 0);
            std::cout << ")" << std::endl;
            break;
        }
        case AsmIRNodeType::CDQ: {
            std::cout << indent << "Cdq()" << std::endl;
            break;
        }
        case AsmIRNodeType::CMP: {
            const auto* cmpNode = static_cast<const AsmIRCmp*>(node);
            std::cout << indent << "Cmp(";
            printIR(cmpNode->operand1.get(), 0);
            std::cout << ", ";
            printIR(cmpNode->operand2.get(), 0);
            std::cout << ")" << std::endl;
            break;
        }
        case AsmIRNodeType::SET_CC: {
            const auto* setCCNode = static_cast<const AsmIRSetCC*>(node);
            std::cout << indent << "SetCC(" << setCCNode->cond_code << ", ";
            printIR(setCCNode->operand.get(), 0);
            std::cout << ")" << std::endl;
            break;
        }
        case AsmIRNodeType::JMP_CC: {
            const auto* jmpCCNode = static_cast<const AsmIRJmpCC*>(node);
            std::cout << indent << "JmpCC(" << jmpCCNode->cond_code << ", " << jmpCCNode->identifier << ")" << std::endl;
            break;
        }
        case AsmIRNodeType::LABEL: {
            const auto* labelNode = static_cast<const AsmIRLabel*>(node);
            std::cout << indent << "Label(" << labelNode->identifier << ")" << std::endl;
            break;
        }
        case AsmIRNodeType::ALLOCATE_STACK: {
            const auto* allocateStackNode = static_cast<const AsmIRAllocateStack*>(node);
            std::cout << indent << "AllocateStack(" << std::to_string(allocateStackNode->stack_size) << ")" << std::endl;
            break;
        }
        case AsmIRNodeType::STACK: {
            const auto* stackNode = static_cast<const AsmIRStack*>(node);
            std::cout << "Stack(" << std::to_string(stackNode->stack_size) << ")";
            break;
        }
        case AsmIRNodeType::NOT: {
            std::cout << "Not()";
            break;
        }
        case AsmIRNodeType::NEG: {
            std::cout << "Neg()";
            break;
        }
        case AsmIRNodeType::ADD: {
            std::cout << "Add()";
            break;
        }
        case AsmIRNodeType::SUBTRACT: {
            std::cout << "Subtract()";
            break;
        }
        case AsmIRNodeType::MULTIPLY: {
            std::cout << "Multiply()";
            break;
        }
        default:
            std::cout << indent << "UnknownNode(type=" << static_cast<int>(node->type) << std::endl;
            break;
    }
}
