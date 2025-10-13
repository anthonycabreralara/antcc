#include "parser.h"
#include "ast.h"
#include "lexer.h"
#include <memory>
#include <iostream>
#include <vector>

/*
FORMER GRAMMER
<program> ::= <function>
<function> ::= "int" <id> "(" ")" "{" <statement> "}"
<statement> ::= "return" <exp> ";"
<exp> ::= <unary_op> <exp> | <int>
<unary_op> ::= "!" | "~" | "-"
*/

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

std::unique_ptr<Node> Parser::getUnOp(std::string s) {
    if (s == "-") {
        return std::make_unique<Negate>();
    } else if (s == "~") {
        return std::make_unique<Complement>();
    }

    return nullptr;
}

std::unique_ptr<Node> Parser::parseExpression() {
    bool valid = true;
    if (check(TokenType::OPEN_PARENTHESIS)) {
        valid = valid && match(TokenType::OPEN_PARENTHESIS);
        auto expression = parseExpression();
        valid = valid && match(TokenType::CLOSE_PARENTHESIS);
        return expression;
    }

    if (check(TokenType::NEGATION) || check(TokenType::BITWISE_COMPLEMENT) || check(TokenType::LOGICAL_NEGATION)) {
        TokenType opType = tokens[current].type;
        std::string opValue = tokens[current].value;
        valid = valid && match(opType);
        auto op = getUnOp(opValue);
        auto expression = parseExpression();
        return std::make_unique<UnOpNode>(std::move(op), std::move(expression));
    }
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
