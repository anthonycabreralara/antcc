#include "asm_ir.h"
#include <vector>
#include <memory>

// -------- AsmIR Node Constructors --------
AsmIRRet::AsmIRRet() {
    type = AsmIRNodeType::RETURN;
}

AsmIRImm::AsmIRImm(std::string v) {
    type = AsmIRNodeType::IMMEDIATE;
    value = std::move(v);
}

AsmIRMov::AsmIRMov(std::unique_ptr<AsmIRNode> s, std::unique_ptr<AsmIRNode> d) {
    type = AsmIRNodeType::MOV;
    src = std::move(s);
    dst = std::move(d);
}

AsmIRReg::AsmIRReg(std::string v) {
    type = AsmIRNodeType::REGISTER;
    value = std::move(v);
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
