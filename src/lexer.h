// Lexer.h
#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <unordered_map>

enum class TokenType {
    IDENTIFIER,
    CONSTANT,
    INT_KEYWORD,
    VOID_KEYWORD,
    RETURN_KEYWORD,
    OPEN_PARENTHESIS,
    CLOSE_PARENTHESIS,
    OPEN_BRACE,
    CLOSE_BRACE,
    SEMICOLON, // ;
    NEGATION, // -
    NOT,
    BITWISE_COMPLEMENT, // ~
    DECREMENT, // --
    ASSIGN, // =
    ADD,
    MULTIPLY,
    DIVIDE,
    REMAINDER,
    AND,
    OR,
    EQUAL,
    NOT_EQUAL,
    LESS_THAN,
    LESS_OR_EQUAL,
    GREATER_THAN,
    GREATER_OR_EQUAL,
    UNKNOWN
};

struct Token {
    TokenType type;
    std::string value;
    Token(TokenType t, const std::string& v);
};

class Lexer {
private:
    std::string input;
    size_t position;

    void initKeywords();
    bool isWhiteSpace(char c);
    bool isAlpha(char c);
    bool isDigit(char c);
    bool isAlphaNumeric(char c);
    std::string getNextWord();
    std::string getNextNumber();

public:
    Lexer(const std::string& source);
    std::vector<Token> tokenize();
    bool isValid() const;
    bool valid;    
    std::unordered_map<std::string, TokenType> keywords;
};

std::string getTokenTypeName(TokenType type);
void printTokens(const std::vector<Token>& tokens);

#endif
