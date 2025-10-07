#include "emitter.h"
#include "asm_ir.h"
#include <iostream>
#include <fstream>

void emit(const AsmIRNode* node, std::ofstream& outf) {
    switch (node->type) {
        case AsmIRNodeType::PROGRAM: {
            std::cout << "program" << std::endl;
            const auto* programNode = static_cast<const AsmIRProgram*>(node);
            emit(programNode->function.get(), outf);
            outf << ".section .note.GNU-stack,\"\",@progbits";
            break;
        }
        case AsmIRNodeType::FUNCTION: {
            std::cout << "function" << std::endl;
            const auto* functionNode = static_cast<const AsmIRFunction*>(node);
            outf << "\t.global " << functionNode->name << "\n";
            outf << functionNode->name << ":\n";

            for (const auto& instr : functionNode->instructions->instructions) {
                emit(instr.get(), outf);
            }

            break;
        }
        case AsmIRNodeType::MOV: {
            std::cout << "mov" << std::endl;
            const auto* moveNode = static_cast<const AsmIRMov*>(node);
            outf << "\tmovl ";
            emit(moveNode->src.get(), outf);
            outf << ", ";
            emit(moveNode->dst.get(), outf);
            outf << "\n";
            break;
        }
        case AsmIRNodeType::IMMEDIATE: {
            const auto* immNode = static_cast<const AsmIRImm*>(node);
            outf << "$" << immNode->value;
            break;
        }
        case AsmIRNodeType::REGISTER: {
            const auto* regNode = static_cast<const AsmIRReg*>(node);
            outf << "%" << regNode->value;
            break;
        }
        case AsmIRNodeType::RETURN: {
            std::cout << "return" << std::endl;
            outf << "\tret\n";
            break;
        }
        default:
            std::cout << "whyyy" << std::endl;
    }
}

void emitCode(const AsmIRNode* node) {
    std::ofstream outf{ "output.s" };
    if (!outf) {
        std::cerr << "Uh oh, output.s could not be opened for writing!\n";
        return;
    }
    emit(node, outf);
}
