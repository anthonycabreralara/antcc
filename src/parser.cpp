#include "parser.h"
#include "ast.h"
#include "lexer.h"
#include <memory>
#include <iostream>
#include <vector>

// -------- Parser Implementation --------
Parser::Parser(std::vector<Token>& tokens) : tokens(tokens) {}

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

std::unique_ptr<Node> Parser::parseExpression() {
    bool valid = true;
    valid = valid && match(TokenType::CONSTANT);
    return std::make_unique<ConstantNode>(tokens[current - 1].value);
}

std::unique_ptr<Node> Parser::parseStatement() {
    bool valid = true;
    valid = valid && match(TokenType::RETURN_KEYWORD);
    auto expression = parseExpression();
    valid = valid && match(TokenType::SEMICOLON);
    return std::make_unique<ReturnNode>(std::move(expression));
}

std::unique_ptr<FunctionNode> Parser::parseFunction() {
    bool valid = true;
    TokenType returnType = TokenType::UNKNOWN;
    std::string name;

    if (check(TokenType::INT_KEYWORD)) {
        returnType = TokenType::INT_KEYWORD;
        valid = valid && match(TokenType::INT_KEYWORD);
    }
    valid = valid && match(TokenType::IDENTIFIER);
    name = tokens[current - 1].value; 

    valid = valid && match(TokenType::OPEN_PARENTHESIS);
    valid = valid && match(TokenType::VOID_KEYWORD);
    valid = valid && match(TokenType::CLOSE_PARENTHESIS);
    valid = valid && match(TokenType::OPEN_BRACE);

    auto statement = parseStatement();

    valid = valid && match(TokenType::CLOSE_BRACE);

    return std::make_unique<FunctionNode>(name, static_cast<int>(returnType), std::move(statement));
}

std::unique_ptr<ProgramNode> Parser::parseProgram() {
    auto func = parseFunction();
    return std::make_unique<ProgramNode>(std::move(func));
}

std::unique_ptr<Node> Parser::parse() {
    return parseProgram();
}
