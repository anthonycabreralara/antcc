#include "emitter.h"
#include "asm_ir.h"
#include <iostream>
#include <fstream>

void emit(const AsmIRNode* node, std::ofstream& outf) {
    switch (node->type) {
        case AsmIRNodeType::PROGRAM: {
            const auto* programNode = static_cast<const AsmIRProgram*>(node);
            emit(programNode->function.get(), outf);
            outf << ".section .note.GNU-stack,\"\",@progbits";
            break;
        }
        case AsmIRNodeType::FUNCTION: {
            const auto* functionNode = static_cast<const AsmIRFunction*>(node);
            outf << "\t.global " << functionNode->name << "\n";
            outf << functionNode->name << ":\n";
            outf << "\tpushq %rbp\n";
            outf << "\tmovq %rsp, %rbp\n";
            for (const auto& instr : functionNode->instructions->instructions) {
                emit(instr.get(), outf);
            }

            break;
        }
        case AsmIRNodeType::MOV: {
            const auto* moveNode = static_cast<const AsmIRMov*>(node);
            outf << "\tmovl ";
            emit(moveNode->src.get(), outf);
            outf << ", ";
            emit(moveNode->dst.get(), outf);
            outf << "\n";
            break;
        }
        case AsmIRNodeType::UNARY: {
            const auto* unaryNode = static_cast<const AsmIRUnary*>(node);
            outf << "\t";
            emit(unaryNode->unary_operator.get(), outf);
            outf << " ";
            emit(unaryNode->operand.get(), outf);
            outf << "\n";
            break;
        }
        case AsmIRNodeType::ALLOCATE_STACK: {
            // subq $<int>, %rsp
            const auto* allocateStackNode = static_cast<const AsmIRAllocateStack*>(node);
            outf << "\tsubq $" << std::to_string(allocateStackNode->stack_size) << ", %rsp\n";
            break;
        }
        case AsmIRNodeType::REGISTER: {
            const auto* regNode = static_cast<const AsmIRReg*>(node);
            if (regNode->value == "AX") {
                outf << "%eax";
            } else {
                outf << "%r10d";
            }
            break;
        }
        case AsmIRNodeType::STACK: {
            const auto* stackNode = static_cast<const AsmIRStack*>(node);
            outf << std::to_string(stackNode->stack_size) << "(%rbp)";
            break;
        }
        case AsmIRNodeType::IMMEDIATE: {
            const auto* immNode = static_cast<const AsmIRImm*>(node);
            outf << "$" << immNode->value;
            break;
        }
        case AsmIRNodeType::NEG: {
            outf << "negl";
            break;
        }
        case AsmIRNodeType::NOT: {
            outf << "notl";
            break;
        }
        case AsmIRNodeType::RETURN: {
            outf << "\tmovq %rbp, %rsp\n";
            outf << "\tpopq %rbp\n";
            outf << "\tret\n";
            break;
        }
        default:
            std::cout << "Invalid node." << std::endl;
    }
}

void emitCode(const AsmIRNode* node, std::string filename) {
    std::ofstream outf{ filename + ".s" };
    if (!outf) {
        std::cerr << "Uh oh, output.s could not be opened for writing!\n";
        return;
    }
    emit(node, outf);
}