#ifndef AST_H
#define AST_H

#include <string>
#include <memory>

enum class NodeType { PROGRAM, FUNCTION, RETURN, CONSTANT };

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

#endif
