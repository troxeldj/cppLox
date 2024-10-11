#ifndef __TOKEN_HPP__
#define __TOKEN_HPP__
#include <iostream>
#include <any>
#include "TokenType.hpp"

class Token {
public:
  TokenType type;
  std::string lexeme;
  std::any literal;
  int line;

  Token(TokenType, std::string&, std::any, int);
  Token(TokenType, const std::string&, std::any, int);
  Token(const Token&);
  std::string toString();
};

#endif