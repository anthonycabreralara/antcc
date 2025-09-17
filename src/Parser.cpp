#include "Parser.h"
#include "Lexer.h"
#include <iostream>
#include <vector>

// Constructor for ReturnNode
ReturnNode::ReturnNode(Node* e) {
    type = RETURN;
    expr = e;
}

// Constructor for NumberNode
NumberNode::NumberNode(int v) {
    type = NUMBER;
    value = v;
}

// Constructor for FunctionNode
FunctionNode::FunctionNode(std::string n, TokenType r) {
    type = FUNCTION; // Placeholder, adjust as needed
    name = n;
    returnType = r;
}


Node parse(std::vector<Token>& tokens) {
    std::cout << "Parsing not yet implemented." << std::endl;
    return Node();
}
