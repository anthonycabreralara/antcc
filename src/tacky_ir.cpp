#include "tacky_ir.h"

// -------- Tacky IR Node Constructors --------
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
    value = v;
}

TackyIRVar::TackyIRVar(std::string v) {
    type = TackyIRNodeType::VAR;
    value = v;
}

TackyIRUnary::TackyIRUnary(std::unique_ptr<TackyIRNode> o, std::unique_ptr<TackyIRNode> s, std::unique_ptr<TackyIRNode> d) {
    op = std::move(o);
    src = std::move(s);
    dst = std::move(d);
}