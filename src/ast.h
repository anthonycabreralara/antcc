#ifndef AST_H
#define AST_H

#include <string>
#include <memory>

enum class NodeType { PROGRAM, FUNCTION, UNARY_OP, RETURN, CONSTANT };

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

class UnOpNode : public Node {
public:
    std::string op;
    std::unique_ptr<Node> expr;
    UnOpNode(std::string o, std::unique_ptr<Node> e);
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

void printTabs(int count);
void printAST(const Node* node, int count);

#endif
