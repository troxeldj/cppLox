#ifndef __SCANNER_HPP__
#define __SCANNER_HPP__
#include <iostream>
#include <vector>
#include "Token.hpp"
#include <map>

class Scanner {
private:
  std::vector<Token> tokens;
  static std::map<std::string, TokenType> keywords; 
  std::string source;
  int start;
  int current;
  int line;
  
  Scanner() = delete;

  bool isDigit(char c);
  bool isAlpha(char c);
  bool isAlphaNumeric(char c);
  bool match(char expected);
  char peek();
  void addToken(TokenType type);
  void addToken(TokenType type, std::string& value);
  Token scanToken();
  bool isAtEnd();
  char advance();
  void string();
  void number();
  void identifier();

public:
    Scanner(std::string& source);
    ~Scanner();
    std::vector<Token>& scanTokens();
};

#endif