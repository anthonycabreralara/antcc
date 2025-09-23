#include "Parser.h"
#include "Lexer.h"
#include <iostream>
#include <vector>

// -------- Node Constructors --------

// Constructor for ReturnNode
ReturnNode::ReturnNode(Node* e) {
    type = RETURN;
    expr = e;
}

// Constructor for ConstantNode
ConstantNode::ConstantNode(int v) {
    type = NUMBER;
    value = v;
}

// Constructor for FunctionNode
FunctionNode::FunctionNode(std::string n, TokenType r) {
    type = FUNCTION;
    name = n;
    returnType = r;
}

// Constructor for ProgramNode
ProgramNode::ProgramNode(FunctionNode* func) {
    type = PROGRAM;
    function = func;
}

// -------- Parser Implementation --------

Parser::Parser(std::vector<Token>& tokens) : tokens(tokens) {
    std::cout << "Parser initialized with " << tokens.size() << " tokens.\n";
}

bool Parser::isAtEnd() const {
    return current >= tokens.size();
}

Token& Parser::advance() {
    if (!isAtEnd()) current++;
    return tokens[current - 1];
}

bool Parser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return tokens[current].type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

FunctionNode* Parser::parseFunction() {
    return nullptr;
}

ProgramNode* Parser::parseProgram() {
    FunctionNode* func = parseFunction();
    return new ProgramNode(func);
}

Node* Parser::parse() {
    // Placeholder for actual parsing logic
    std::cout << "Parsing not yet implemented.\n";
    return parseProgram();
}
