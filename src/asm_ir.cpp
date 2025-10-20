#include "asm_ir.h"
#include <memory>

// -------- AsmIR Node Constructors --------
AsmIRRet::AsmIRRet() {
    type = AsmIRNodeType::RETURN;
}

AsmIRMov::AsmIRMov(std::unique_ptr<AsmIRNode> s, std::unique_ptr<AsmIRNode> d) {
    type = AsmIRNodeType::MOV;
    src = std::move(s);
    dst = std::move(d);
}

AsmIRUnary::AsmIRUnary(std::unique_ptr<AsmIRNode> unary_operator, std::unique_ptr<AsmIRNode> operand) 
    : unary_operator(std::move(unary_operator)), operand(std::move(operand)) {
    type = AsmIRNodeType::UNARY;
}

AsmIRAllocateStack::AsmIRAllocateStack(int stack_size) 
    : stack_size(stack_size) {
    type = AsmIRNodeType::ALLOCATE_STACK;
}

AsmIRNeg::AsmIRNeg() {
    type = AsmIRNodeType::NEG;
}

AsmIRNot::AsmIRNot() {
    type = AsmIRNodeType::NOT;
}

AsmIRImm::AsmIRImm(std::string v) {
    type = AsmIRNodeType::IMMEDIATE;
    value = std::move(v);
}

AsmIRReg::AsmIRReg(std::string v) {
    type = AsmIRNodeType::REGISTER;
    value = std::move(v);
}

AsmIRPseudo::AsmIRPseudo(std::string identifier)
    : identifier(identifier) {
    type = AsmIRNodeType::PSEUDO;
}

AsmIRStack::AsmIRStack(int stack_size)
    : stack_size(stack_size) {
    type = AsmIRNodeType::STACK;
}

AsmIRInstructions::AsmIRInstructions() {}

AsmIRFunction::AsmIRFunction(std::string n, std::unique_ptr<AsmIRInstructions> instr)
    : name(std::move(n)), instructions(std::move(instr)) {
    type = AsmIRNodeType::FUNCTION;
}

AsmIRProgram::AsmIRProgram(std::unique_ptr<AsmIRFunction> func) {
    type = AsmIRNodeType::PROGRAM;
    function = std::move(func);
}
