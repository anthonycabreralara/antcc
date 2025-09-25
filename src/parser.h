#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include "lexer.h" 

class Node {
public:
    enum Type { PROGRAM, FUNCTION, RETURN, CONSTANT };
    Type type;
    virtual ~Node() = default;
};

class ReturnNode : public Node {
public:
    Node* expr;
    ReturnNode(Node* e);
};

class ConstantNode : public Node {
public:
    std::string value;
    ConstantNode(std::string v);
};

class FunctionNode : public Node {
public:
    std::string name;
    TokenType returnType;
    Node* statement;
    FunctionNode(std::string n, TokenType r, Node* stmt);
};

class ProgramNode : public Node {
public:
    FunctionNode* function;
    ProgramNode(FunctionNode* func);
};

class Parser {
private:
    int current = 0;
    std::vector<Token>& tokens;
    Node* parseExpression();
    Node* parseStatemant();
    ProgramNode* parseProgram();
    FunctionNode* parseFunction();
    bool isAtEnd() const;
    Token& advance();
    Token& peek();
    bool check(TokenType type) const;
    bool match(TokenType type);

public:
    Parser(std::vector<Token>& tokens);
    Node* parse(); // returns AST root
};

#endif
