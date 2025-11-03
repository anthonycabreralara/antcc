#include "parser.h"
#include "ast.h"
#include "lexer.h"
#include <memory>
#include <vector>

/*
FORMER GRAMMER
<program>       ::= <function>
<function>      ::= "int" <id> "(" ")" "{" <statement> "}"
<statement>     ::= "return" <exp> ";"
<exp>           ::= <factor> | <exp> <binop> <exp>
<factor>        ::= <int> | <unop> <factor> | "(" <exp> ")" 
<unop>          ::= "-" | "~"
<binop>         ::= "-" | "+" | "*" | "/" | "%"
<identifier>    ::= ? An identifier token ?
<int>           ::= ? A constant token ? 
*/

// -------- Parser Implementation --------
Parser::Parser(std::vector<Token>& tokens) : tokens(tokens), valid(true) {}

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

std::unique_ptr<Node> Parser::getBinOp(std::string s) {
    if (s == "+") {
        return std::make_unique<AddNode>();
    } else if (s == "-") {
        return std::make_unique<SubtractNode>();
    } else if (s == "*") {
        return std::make_unique<MultiplyNode>();
    } else if (s == "/") {
        return std::make_unique<DivideNode>();
    } else if (s == "%") {
        return std::make_unique<RemainderNode>();
    }

    return nullptr;
}

int Parser::getPrecidence(std::string s) {
    if (s == "+") {
        return 45;
    } else if (s == "-") {
        return 45;
    } else if (s == "*") {
        return 50;
    } else if (s == "/") {
        return 50;
    } else if (s == "%") {
        return 50;
    }

    return 0;
}

std::unique_ptr<Node> Parser::parseFactor() {
    if (check(TokenType::CONSTANT)) {
        valid = valid && match(TokenType::CONSTANT);
        return std::make_unique<ConstantNode>(tokens[current - 1].value);
    } else if (check(TokenType::NEGATION) || check(TokenType::BITWISE_COMPLEMENT)) {
        TokenType opType = tokens[current].type;
        std::string opValue = tokens[current].value;
        valid = valid && match(opType);
        auto op = getUnOp(opValue);
        auto expression = parseFactor();
        return std::make_unique<UnOpNode>(std::move(op), std::move(expression));
    } else if (check(TokenType::OPEN_PARENTHESIS)) {
        valid = valid && match(TokenType::OPEN_PARENTHESIS);
        auto expression = parseExpression(0);
        valid = valid && match(TokenType::CLOSE_PARENTHESIS);
        return expression;
    }

    return nullptr;

}

std::unique_ptr<Node> Parser::parseExpression(int minPrec) {
    auto left = parseFactor();
    while ((check(TokenType::ADD) || check(TokenType::NEGATION) || 
            check(TokenType::MULTIPLY) || check(TokenType::DIVIDE) ||
            check(TokenType::REMAINDER)) && getPrecidence(tokens[current].value) >= minPrec) {
        TokenType opType = tokens[current].type;
        std::string opValue = tokens[current].value;
        valid = valid && match(opType);
        auto op = getBinOp(opValue);
        auto right = parseExpression(getPrecidence(opValue) + 1);
        left = std::make_unique<BinaryNode>(std::move(op), std::move(left), std::move(right));
    }
    return std::move(left);

}

std::unique_ptr<Node> Parser::parseStatement() {
    valid = valid && match(TokenType::RETURN_KEYWORD);
    auto expression = parseExpression(0);
    valid = valid && match(TokenType::SEMICOLON);
    return std::make_unique<ReturnNode>(std::move(expression));
}

std::unique_ptr<FunctionNode> Parser::parseFunction() {
    TokenType returnType = TokenType::UNKNOWN;
    std::string name;

    if (check(TokenType::INT_KEYWORD)) {
        returnType = TokenType::INT_KEYWORD;
        valid = valid && match(TokenType::INT_KEYWORD);
    } else {
        valid = false;
        return nullptr;
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
