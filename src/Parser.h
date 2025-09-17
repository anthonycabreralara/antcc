#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include "Lexer.h" 

class Node {
public:
    enum Type { RETURN, BINARY_OP, NUMBER, FUNCTION };
    Type type;
    virtual ~Node() = default;
};

class ReturnNode : public Node {
public:
    Node* expr;
    ReturnNode(Node* e);
};

class NumberNode : public Node {
public:
    int value;
    NumberNode(int v);
};

class FunctionNode : public Node {
public:
    std::string name;
    TokenType returnType;
    FunctionNode(std::string n, TokenType r);
};

Node parse(std::vector<Token>& tokens);
#endif
