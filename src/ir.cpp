#include "ir.h"
#include <vector>
#include <memory>

// -------- IR Node Constructors --------
IRRet::IRRet() {
    type = IRNodeType::RETURN;
}

IRImm::IRImm(std::string v) {
    type = IRNodeType::IMMEDIATE;
    value = std::move(v);
}

IRMov::IRMov(std::unique_ptr<IRNode> s, std::unique_ptr<IRNode> d) {
    type = IRNodeType::MOV;
    src = std::move(s);
    dst = std::move(d);
}

IRReg::IRReg(std::string v) {
    type = IRNodeType::REGISTER;
    value = std::move(v);
}

IRInstructions::IRInstructions() {}

IRFunction::IRFunction(std::string n, std::unique_ptr<IRInstructions> instr)
    : name(std::move(n)), instructions(std::move(instr)) {}


IRProgram::IRProgram(std::unique_ptr<IRFunction> func) {
    type = IRNodeType::PROGRAM;
    function = std::move(func);
}
