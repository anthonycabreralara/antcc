#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

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
  SEMICOLON
};

struct Lexer  {
private:
  string input;
  size_t position;
  unordered_map<string, TokenType> keywords;
  
}