#include "tacky_ir.h"

// -------- Tacky IR Node Constructors --------
TackyIRProgram::TackyIRProgram(std::unique_ptr<TackyIRNode> func) {
    type = TackyIRNodeType::PROGRAM;
    function = std::move(func);
}

TackyIRInstructions::TackyIRInstructions() {}

TackyIRFunction::TackyIRFunction(std::string n, std::unique_ptr<TackyIRInstructions> instr)
    : name(std::move(n)), instructions(std::move(instr)) {
    type = TackyIRNodeType::FUNCTION;
}

TackyIRReturn::TackyIRReturn(std::unique_ptr<TackyIRNode> e) {
    type = TackyIRNodeType::RETURN;
    expr = std::move(e);
}

TackyIRComplement::TackyIRComplement() {
    type = TackyIRNodeType::COMPLEMENT;
}

TackyIRNegate::TackyIRNegate() {
    type = TackyIRNodeType::NEGATE;
}

TackyIRConstant::TackyIRConstant(std::string v) {
    type = TackyIRNodeType::CONSTANT;
    value = std::move(v);
}

TackyIRVar::TackyIRVar(std::string v) {
    type = TackyIRNodeType::VAR;
    value = std::move(v);
}

TackyIRUnary::TackyIRUnary(std::unique_ptr<TackyIRNode> o, std::unique_ptr<TackyIRNode> s, std::unique_ptr<TackyIRNode> d) {
    type = TackyIRNodeType::UNARY;
    op = std::move(o);
    src = std::move(s);
    dst = std::move(d);
}