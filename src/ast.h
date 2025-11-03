#ifndef AST_H
#define AST_H

#include <string>
#include <memory>

enum class NodeType { 
    PROGRAM, 
    FUNCTION, 
    UNARY_OP, 
    BINARY_OP, 
    RETURN, 
    CONSTANT, 
    NEGATE, 
    COMPLEMENT, 
    ADD, 
    SUBTRACT, 
    MULTIPLY, 
    DIVIDE, 
    REMAINDER, 
    AND, 
    OR,
    EQUAL,
    NOT_EQUAL,
    LESS_THAN,
    LESS_OR_EQUAL,
    GREATER_THAN,
    GREATER_OR_EQUAL,

};

/*
program = Program(function_declaration)
function_declaration = Function(string, statement) //string is the function name
statement = Return(exp)
exp = UnOp(operator, exp) | Constant(int)
*/

class Node {
public:
    NodeType type;
    virtual ~Node() = default;
};

class ReturnNode : public Node {
public:
    std::unique_ptr<Node> expr;
    ReturnNode(std::unique_ptr<Node> e);
};

class Complement : public Node {
public:
    Complement();
};

class Negate : public Node {
public:
    Negate();
};

class UnOpNode : public Node {
public:
    std::unique_ptr<Node> op;
    std::unique_ptr<Node> expr;
    UnOpNode(std::unique_ptr<Node> o, std::unique_ptr<Node> e);
};

class BinaryNode : public Node {
public:
    std::unique_ptr<Node> binaryOperator;
    std::unique_ptr<Node> expression1;
    std::unique_ptr<Node> expression2;
    BinaryNode(std::unique_ptr<Node> binaryOperator, std::unique_ptr<Node> expression1, std::unique_ptr<Node> expression2);
};

class AddNode : public Node {
public:
    AddNode();
};

class SubtractNode : public Node {
public:
    SubtractNode();
};

class MultiplyNode : public Node {
public:
    MultiplyNode();
};

class DivideNode : public Node {
public:
    DivideNode();
};

class RemainderNode : public Node {
public:
    RemainderNode();
};

class AndNode : public Node {
public:
    AndNode();
};

class OrNode : public Node {
public:
    OrNode();
};

class EqualNode : public Node {
public:
    EqualNode();
};

class NotEqualNode : public Node {
public:
    NotEqualNode();
};

class LessThanNode : public Node {
public:
    LessThanNode();
};

class LessOrEqualNode : public Node {
public:
    LessOrEqualNode();
};

class GreaterThanNode : public Node {
public:
    GreaterThanNode();
};

class GreaterOrEqualNode : public Node {
public:
    GreaterOrEqualNode();
};

class ConstantNode : public Node {
public:
    std::string value;
    ConstantNode(std::string v);
};

class FunctionNode : public Node {
public:
    std::string name;
    int returnType; // Or TokenType if you include lexer.h here
    std::unique_ptr<Node> statement;
    FunctionNode(std::string n, int r, std::unique_ptr<Node> stmt);
};

class ProgramNode : public Node {
public:
    std::unique_ptr<FunctionNode> function;
    ProgramNode(std::unique_ptr<FunctionNode> func);
};

void printSpace(int count);
void printAST(const Node* node, int count);

#endif
