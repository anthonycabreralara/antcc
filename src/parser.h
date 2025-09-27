#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <memory>
#include <string>
#include "lexer.h"
#include "ast.h"

class Parser {
private:
    int current = 0;
    std::vector<Token>& tokens;

    std::unique_ptr<Node> parseExpression();
    std::unique_ptr<Node> parseStatement();
    std::unique_ptr<ProgramNode> parseProgram();
    std::unique_ptr<FunctionNode> parseFunction();

    bool isAtEnd() const;
    Token& advance();
    bool check(TokenType type) const;
    bool match(TokenType type);

public:
    Parser(std::vector<Token>& tokens);
    std::unique_ptr<Node> parse(); // returns AST root
};

#endif
