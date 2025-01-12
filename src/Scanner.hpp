#ifndef ___SCANNER_HPP
#define ___SCANNER_HPP
#include "Token.hpp"
#include <iostream>
#include <map>
#include <vector>

class Scanner {
private:
  static std::map<std::string, TokenType> keywords;
  std::string content;
  int current;
  int start;
  int line;

public:
  std::vector<Token> tokens;
  Scanner() = delete;
  Scanner(std::string content)
    : content(std::move(content)), start(0), current(0), line(1) {}
  std::vector<Token>& scanTokens();
private:
  char consume();
  char peek();
  char peekNext();
  void scanToken();
  bool isAtEnd();
  void addToken(TokenType type);
  bool match(char expected);

  void string();
  void identifier();
  void number();


  bool isDigit(char c);
  bool isAlpha(char c);
  bool isAlphaNumeric(char c);
};
#endif