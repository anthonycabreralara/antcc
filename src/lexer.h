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
    BITWISE_COMPLEMENT, // ~
    DECREMENT, // ~
    LOGICAL_NEGATION, // !
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
    std::unordered_map<std::string, TokenType> keywords;

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
};

std::string getTokenTypeName(TokenType type);
void printTokens(const std::vector<Token>& tokens);

#endif