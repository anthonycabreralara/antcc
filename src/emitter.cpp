#include "emitter.h"
#include "asm_ir.h"
#include <iostream>
#include <fstream>

void emit(const AsmIRNode* node, std::ofstream& outf, int registerBytes) {
    switch (node->type) {
        case AsmIRNodeType::PROGRAM: {
            const auto* programNode = static_cast<const AsmIRProgram*>(node);
            emit(programNode->function.get(), outf, 4);
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
                emit(instr.get(), outf, 4);
            }

            break;
        }
        case AsmIRNodeType::MOV: {
            const auto* moveNode = static_cast<const AsmIRMov*>(node);
            outf << "\tmovl ";
            emit(moveNode->src.get(), outf, 4);
            outf << ", ";
            emit(moveNode->dst.get(), outf, 4);
            outf << "\n";
            break;
        }
        case AsmIRNodeType::UNARY: {
            const auto* unaryNode = static_cast<const AsmIRUnary*>(node);
            outf << "\t";
            emit(unaryNode->unary_operator.get(), outf, 4);
            outf << " ";
            emit(unaryNode->operand.get(), outf, 4);
            outf << "\n";
            break;
        }
        case AsmIRNodeType::BINARY: {
            const auto* binaryNode = static_cast<const AsmIRBinary*>(node);
            outf << "\t";
            emit(binaryNode->binary_operator.get(), outf, 4);
            outf << " ";
            emit(binaryNode->operand1.get(), outf, 4);
            outf << ", ";
            emit(binaryNode->operand2.get(), outf, 4);
            outf << "\n";
            break;
        }
        case AsmIRNodeType::CMP: {
            const auto* cmpNode = static_cast<const AsmIRCmp*>(node);
            outf << "\t";
            outf << "cmpl ";
            emit(cmpNode->operand1.get(), outf, 4);
            outf << ", ";
            emit(cmpNode->operand2.get(), outf, 4);
            outf << "\n";
            break;
        }
        case AsmIRNodeType::JMP: {
            const auto* jmpNode = static_cast<const AsmIRJmp*>(node);
            outf << "\t";
            outf << "jmp .L" << jmpNode->identifier; 
            outf << "\n";
            break;
        }
        case AsmIRNodeType::JMP_CC: {
            const auto* jmpNode = static_cast<const AsmIRJmpCC*>(node);
            std::string cond_code = "";
            if (jmpNode->cond_code == "NE") {
                cond_code = "ne";
            } else if (jmpNode->cond_code == "E") {
                cond_code = "e";
            }
            outf << "\t";
            outf << "j" << cond_code << " .L" << jmpNode->identifier ;
            outf << "\n";
            break;
        }
        case AsmIRNodeType::SET_CC: {
            const auto* setCCNode = static_cast<const AsmIRSetCC*>(node);
            std::string cond_code = "";
            if (setCCNode->cond_code == "E") {
                cond_code = "e";
            } else if (setCCNode->cond_code == "NE") {
                cond_code = "ne";
            } else if (setCCNode->cond_code == "L") {
                cond_code = "l";
            } else if (setCCNode->cond_code == "LE") {
                cond_code = "le";
            } else if (setCCNode->cond_code == "G") {
                cond_code = "g";
            } else if (setCCNode->cond_code == "GE") {
                cond_code = "ge";
            } else {
                std::cout << "Invalid condition code" << std::endl;
            }
            outf << "\t";
            outf << "set" << cond_code << " ";
            emit(setCCNode->operand.get(), outf, 1);
            outf << "\n";
            break;
        }
        case AsmIRNodeType::LABEL: {
            const auto* labelNode = static_cast<const AsmIRLabel*>(node);
            outf << "\t";
            outf << ".L" << labelNode->identifier << ":";
            outf << "\n";
            break;
        }
        case AsmIRNodeType::IDIV: {
            const auto* idiv = static_cast<const AsmIRIdiv*>(node);
            outf << "\t";
            outf << "idivl ";
            emit(idiv->operand.get(), outf, 4);
            outf << "\n";

            break;
        }
        case AsmIRNodeType::CDQ: {
            const auto* cdq = static_cast<const AsmIRCdq*>(node);
            outf << "\t";
            outf << "cdq";
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
            if (regNode->value == "AX" && registerBytes == 4) {
                outf << "%eax";
            } else if (regNode->value == "AX" && registerBytes == 1) {
                outf << "%al";
            } else if (regNode->value == "DX" && registerBytes == 4) { 
                outf << "%edx";
            } else if (regNode->value == "DX" && registerBytes == 1) { 
                outf << "%dl";
            } else if (regNode->value == "R10" && registerBytes == 4) {
                outf << "%r10d";
            } else if (regNode->value == "R10" && registerBytes == 1) {
                outf << "%r10b";
            } else if (regNode->value == "R11" && registerBytes == 4) {
                outf << "%r11d";
            } else if (regNode->value == "R11" && registerBytes == 1) {
                outf << "%r11b";
            } else {
                outf << "%";
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
        case AsmIRNodeType::ADD: {
            outf << "addl";
            break;
        }
        case AsmIRNodeType::SUBTRACT: {
            outf << "subl";
            break;
        }
        case AsmIRNodeType::MULTIPLY: {
            outf << "imull";
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
    // registerBytes default to 4, exceptions in setcc
    emit(node, outf, 4);
}