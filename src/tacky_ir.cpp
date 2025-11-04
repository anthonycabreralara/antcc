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

TackyIRNot::TackyIRNot() {
    type = TackyIRNodeType::NOT;
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

TackyIREqual::TackyIREqual() {
    type = TackyIRNodeType::EQUAL;
}

TackyIRNotEqual::TackyIRNotEqual() {
    type = TackyIRNodeType::NOT_EQUAL;
}

TackyIRLessThan::TackyIRLessThan() {
    type = TackyIRNodeType::LESS_THAN;
}

TackyIRLessOrEqual::TackyIRLessOrEqual() {
    type = TackyIRNodeType::LESS_OR_EQUAL;
}

TackyIRGreaterThan::TackyIRGreaterThan() {
    type = TackyIRNodeType::GREATER_THAN;
}

TackyIRGreaterOrEqual::TackyIRGreaterOrEqual() {
    type = TackyIRNodeType::GREATER_OR_EQUAL;
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

TackyIRCopy::TackyIRCopy(std::unique_ptr<TackyIRNode> src, std::unique_ptr<TackyIRNode> dst)
    : src(std::move(src)), dst(std::move(dst)) {
    type = TackyIRNodeType::COPY;
}

TackyIRJump::TackyIRJump(std::unique_ptr<TackyIRNode> target)
    : target(std::move(target)) {
    type = TackyIRNodeType::JUMP;
}

TackyIRJumpIfZero::TackyIRJumpIfZero(std::unique_ptr<TackyIRNode> condition, std::unique_ptr<TackyIRNode> target)
    : condition(std::move(condition)), target(std::move(target)) {
    type = TackyIRNodeType::JUMP_IF_ZERO;
}

TackyIRJumpIfNotZero::TackyIRJumpIfNotZero(std::unique_ptr<TackyIRNode> condition, std::unique_ptr<TackyIRNode> target)
    : condition(std::move(condition)), target(std::move(target)) {
    type = TackyIRNodeType::JUMP_IF_NOT_ZERO;
}

TackyIRLabel::TackyIRLabel(std::unique_ptr<TackyIRNode> identifier)
    : identifier(std::move(identifier)) {
    type = TackyIRNodeType::LABEL;
}