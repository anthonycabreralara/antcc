#include "Parser.h"
#include "Lexer.h"
#include <iostream>
#include <vector>

/* FORMAL GRAMMER 
<program> ::= <function> 
<function> ::= "int" <identifier> "(" "void" ")" "{" <statement> "}" 
<statement> ::= "return" <expr> ";" 
<expr> ::= <int> 
<identifier> ::= ? An identifier token ? 
<int> ::= ? A constant token ? */

// -------- Node Constructors --------

// Constructor for ReturnNode
ReturnNode::ReturnNode(Node* e) {
    type = RETURN;
    expr = e;
}

// Constructor for ConstantNode
ConstantNode::ConstantNode(std::string v) {
    type = NUMBER;
    value = v;
}

// Constructor for FunctionNode
FunctionNode::FunctionNode(std::string n, TokenType r, Node* stmt) {
    type = FUNCTION;
    name = n;
    returnType = r;
    statement = stmt;
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


Node* Parser::parseExpression() {
  bool valid = true;
  valid = valid && match(TokenType::CONSTANT);
  return new ConstantNode(tokens[current-1].value);
}

Node* Parser::parseStatemant() {
    bool valid = true;
    valid = valid && match(TokenType::RETURN_KEYWORD);
    Node* expression = parseExpression();
    valid = valid && match(TokenType::SEMICOLON);
    return new ReturnNode(expression);
}

FunctionNode* Parser::parseFunction() {
    bool valid = true;
    TokenType returnType = TokenType::UNKNOWN;
    std::string name = "";

    if (check(TokenType::INT_KEYWORD)) {
        returnType = TokenType::INT_KEYWORD;
        valid = valid && match(TokenType::INT_KEYWORD);
    }
    valid = valid && match(TokenType::IDENTIFIER);
    name = tokens[current-1].value; 

    valid = valid && match(TokenType::OPEN_PARENTHESIS);
    valid = valid && match(TokenType::VOID_KEYWORD);
    valid = valid && match(TokenType::CLOSE_PARENTHESIS);
    valid = valid && match(TokenType::OPEN_BRACE);

    Node* statement = parseStatemant();

    valid = valid && match(TokenType::CLOSE_BRACE);

    std::cout << "valid: " << valid << std::endl;
    std::cout << "current: " << current << std::endl;

    return new FunctionNode(name, returnType, statement);
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
