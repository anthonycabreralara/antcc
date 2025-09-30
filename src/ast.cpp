#include "ast.h"

// -------- AST Node Constructors --------
ReturnNode::ReturnNode(std::unique_ptr<Node> e) {
    type = NodeType::RETURN;
    expr = std::move(e);
}

UnOpNode::UnOpNode(std::string o, std::unique_ptr<Node> e) {
    type = NodeType::UNARY_OP;
    op = o;
    expr = std::move(e);
}

ConstantNode::ConstantNode(std::string v) {
    type = NodeType::CONSTANT;
    value = std::move(v);
}

FunctionNode::FunctionNode(std::string n, int r, std::unique_ptr<Node> stmt) {
    type = NodeType::FUNCTION;
    name = std::move(n);
    returnType = r;
    statement = std::move(stmt);
}

ProgramNode::ProgramNode(std::unique_ptr<FunctionNode> func) {
    type = NodeType::PROGRAM;
    function = std::move(func);
}
