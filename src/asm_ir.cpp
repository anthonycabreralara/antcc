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

AsmIRBinary::AsmIRBinary(std::unique_ptr<AsmIRNode> binary_operator, std::unique_ptr<AsmIRNode> operand1, std::unique_ptr<AsmIRNode> operand2)
    : binary_operator(std::move(binary_operator)), operand1(std::move(operand1)), operand2(std::move(operand2)) {
    type = AsmIRNodeType::BINARY;
}

AsmIRCmp::AsmIRCmp(std::unique_ptr<AsmIRNode> operand1, std::unique_ptr<AsmIRNode> operand2)
    : operand1(std::move(operand1)), operand2(std::move(operand2)) {
    type = AsmIRNodeType::CMP;
}

AsmIRIdiv::AsmIRIdiv(std::unique_ptr<AsmIRNode> operand)
    : operand(std::move(operand)) {
    type = AsmIRNodeType::IDIV;
}

AsmIRCdq::AsmIRCdq() {
    type = AsmIRNodeType::CDQ;
}

AsmIRJmp::AsmIRJmp(std::string identifier)
    : identifier(std::move(identifier)) {
    type = AsmIRNodeType::JMP;
}

AsmIRJmpCC::AsmIRJmpCC(std::string cond_code, std::string identifier)
    : cond_code(std::move(cond_code)), identifier(std::move(identifier)) {
    type = AsmIRNodeType::JMP_CC;
}

AsmIRSetCC::AsmIRSetCC(std::string cond_code, std::unique_ptr<AsmIRNode> operand)
    : cond_code(std::move(cond_code)), operand(std::move(operand)) {
    type = AsmIRNodeType::SET_CC;
}

AsmIRLabel::AsmIRLabel(std::string identifier) 
    : identifier(std::move(identifier)) {
    type = AsmIRNodeType::LABEL;
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

AsmIRAdd::AsmIRAdd() {
    type = AsmIRNodeType::ADD;
}

AsmIRSubtract::AsmIRSubtract() {
    type = AsmIRNodeType::SUBTRACT;
}

AsmIRMultiply::AsmIRMultiply() {
    type = AsmIRNodeType::MULTIPLY;
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
