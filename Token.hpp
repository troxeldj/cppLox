#ifndef __TOKEN_HPP__
#define __TOKEN_HPP__
#include "TokenType.hpp"
#include <iostream>

class Token {
  TokenType type;
  std::string lexeme;
  std::string literal;
  int line;

public:
  Token(TokenType, const std::string&, const std::string&, int);
  Token(const Token&);
  std::string toString();
};

#endif