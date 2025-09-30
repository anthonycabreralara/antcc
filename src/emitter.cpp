#include "emitter.h"
#include "ir.h"
#include <iostream>
#include <fstream>



void emit(const IRNode* node, std::ofstream& outf) {
    switch (node->type) {
        case IRNodeType::PROGRAM: {
            std::cout << "program" << std::endl;
            const auto* programNode = static_cast<const IRProgram*>(node);
            emit(programNode->function.get(), outf);
            outf << ".section .note.GNU-stack,"",@progbits";
            break;
        }
        case IRNodeType::FUNCTION: {
            std::cout << "function" << std::endl;
            const auto* functionNode = static_cast<const IRFunction*>(node);
            outf << "\t.global " << functionNode->name << "\n";
            outf << functionNode->name << ":\n";

            for (const auto& instr : functionNode->instructions->instructions) {
                emit(instr.get(), outf);
            }

            break;
        }
        case IRNodeType::MOV: {
            std::cout << "mov" << std::endl;
            const auto* moveNode = static_cast<const IRMov*>(node);
            outf << "\tmovl ";
            emit(moveNode->src.get(), outf);
            outf << ", ";
            emit(moveNode->dst.get(), outf);
            outf << "\n";
            break;
        }
        case IRNodeType::IMMEDIATE: {
            const auto* immNode = static_cast<const IRImm*>(node);
            outf << "$" << immNode->value;
            break;
        }
        case IRNodeType::REGISTER: {
            const auto* regNode = static_cast<const IRReg*>(node);
            outf << "%" << regNode->value;
            break;
        }
        case IRNodeType::RETURN: {
            std::cout << "return" << std::endl;
            outf << "\tret\n";
            break;
        }
        default:
            std::cout << "whyyy" << std::endl;
    }
}

void emitCode(const IRNode* node) {
    std::ofstream outf{ "output.s" };
    if (!outf) {
        std::cerr << "Uh oh, output.s could not be opened for writing!\n";
        return;
    }
    emit(node, outf);
}
