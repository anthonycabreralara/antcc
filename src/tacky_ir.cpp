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

TackyIRAdd::TackyIRAdd() {
    type = TackyIRNodeType::ADD;
}

TackyIRSubtract::TackyIRSubtract() {
    type = TackyIRNodeType::SUBTRACT;
}

TackyIRMultiply::TackyIRMultiply() {
    type = TackyIRNodeType::MULTIPLY;
}

TackyIRDivide::TackyIRDivide() {
    type = TackyIRNodeType::DIVIDE;
}

TackyIRRemainder::TackyIRRemainder() {
    type = TackyIRNodeType::REMAINDER;
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

TackyIRBinary::TackyIRBinary(std::unique_ptr<TackyIRNode> op, std::unique_ptr<TackyIRNode> src1, std::unique_ptr<TackyIRNode> src2, std::unique_ptr<TackyIRNode> dst)
    : op(std::move(op)), src1(std::move(src1)), src2(std::move(src2)), dst(std::move(dst)) {
    type = TackyIRNodeType::BINARY;
}