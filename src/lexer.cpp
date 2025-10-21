#include "lexer.h"
#include <iostream>

Token::Token(TokenType t, const std::string& v) : type(t), value(v) {}

Lexer::Lexer(const std::string& source) : input(source), position(0), valid(true) {
    initKeywords();
}

void Lexer::initKeywords() {
    keywords["int"] = TokenType::INT_KEYWORD;
    keywords["void"] = TokenType::VOID_KEYWORD;
    keywords["return"] = TokenType::RETURN_KEYWORD;
}

bool Lexer::isWhiteSpace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool Lexer::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool Lexer::isDigit(char c) {
    return (c >= '0' && c <= '9');
}

bool Lexer::isAlphaNumeric(char c) {
    return isAlpha(c) || isDigit(c);
}

std::string Lexer::getNextWord() {
    size_t start = position;
    while (position < input.length() && isAlphaNumeric(input[position])) {
        position++;
    }
    return input.substr(start, position - start);
}

std::string Lexer::getNextNumber() {
    size_t start = position;
    while (position < input.length() && isDigit(input[position])) {
        position++;
    }
    return input.substr(start, position - start);
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (position < input.length()) {
        char currentChar = input[position];

        if (isWhiteSpace(currentChar)) {
            position++;
            continue;
        }

        if (isAlpha(currentChar)) {
            std::string word = getNextWord();
            if (keywords.find(word) != keywords.end()) {
                tokens.emplace_back(keywords[word], word);
            } else {
                tokens.emplace_back(TokenType::IDENTIFIER, word);
            }
        }
        else if (isDigit(currentChar)) {
            std::string number = getNextNumber();
            tokens.emplace_back(TokenType::CONSTANT, number);
        }
        else if (currentChar == '(' || currentChar == ')' ||
                 currentChar == '{' || currentChar == '}' ||
                 currentChar == ';' || currentChar == '-' ||
                 currentChar == '~' || currentChar == '!' ||
                 currentChar == '+' || currentChar == '*' ||
                 currentChar == '%') {
            switch (currentChar) {
                case '(': tokens.emplace_back(TokenType::OPEN_PARENTHESIS, "("); break;
                case ')': tokens.emplace_back(TokenType::CLOSE_PARENTHESIS, ")"); break;
                case '{': tokens.emplace_back(TokenType::OPEN_BRACE, "{"); break;
                case '}': tokens.emplace_back(TokenType::CLOSE_BRACE, "}"); break;
                case ';': tokens.emplace_back(TokenType::SEMICOLON, ";"); break;
                case '~': tokens.emplace_back(TokenType::BITWISE_COMPLEMENT, "~"); break;
                case '!': tokens.emplace_back(TokenType::LOGICAL_NEGATION, "!"); break;
                case '-':
                    if (position + 1 < input.length() && input[position + 1] == '-') {
                        tokens.emplace_back(TokenType::DECREMENT, "--");
                        position++;
                        break;
                    } else {
                        tokens.emplace_back(TokenType::NEGATION, "-"); break;
                    }
                case '+' : tokens.emplace_back(TokenType::ADD, "+"); break;
                case '*' : tokens.emplace_back(TokenType::MULTIPLY, "*"); break;
                case '%' : tokens.emplace_back(TokenType::REMAINDER, "%"); break;

            }
            position++;
        }
        else {
            tokens.emplace_back(TokenType::UNKNOWN, std::string(1, currentChar));
            valid = false;
            position++;
        }
    }

    return tokens;
}

bool Lexer::isValid() const {
    return valid;
}

std::string getTokenTypeName(TokenType type) {
    switch (type) {
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::CONSTANT: return "CONSTANT";
        case TokenType::INT_KEYWORD: return "INT_KEYWORD";
        case TokenType::VOID_KEYWORD: return "VOID_KEYWORD";
        case TokenType::RETURN_KEYWORD: return "RETURN_KEYWORD";
        case TokenType::OPEN_PARENTHESIS: return "OPEN_PARENTHESIS";
        case TokenType::CLOSE_PARENTHESIS: return "CLOSE_PARENTHESIS";
        case TokenType::OPEN_BRACE: return "OPEN_BRACE";
        case TokenType::CLOSE_BRACE: return "CLOSE_BRACE";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::NEGATION: return "NEGATION";
        case TokenType::DECREMENT: return "DECREMENT";
        case TokenType::BITWISE_COMPLEMENT: return "BITWISE_COMPLIMENT";
        case TokenType::LOGICAL_NEGATION: return "LOGICAL_NEGATION";
        case TokenType::ADD: return "ADD";
        case TokenType::MULTIPLY: return "MULTIPLY";
        case TokenType::DIVIDE: return "DIVIDE";
        case TokenType::REMAINDER: return "REMAINDER";
        case TokenType::UNKNOWN: return "UNKNOWN";
    }
    return "UNKNOWN";
}

void printTokens(const std::vector<Token>& tokens) {
    for (const auto& token : tokens) {
        std::cout << "Type: " << getTokenTypeName(token.type)
                  << ", Value: " << token.value << std::endl;
    }
}
