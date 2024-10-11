#ifndef __PARSER_HPP
#define __PARSER_HPP
#include <vector>
#include "Token.hpp"

class Expr;

class Parser {
  std::vector<Token> tokens;
  int current;
  Parser() = delete; 

public:
  Parser(std::vector<Token>);
  Expr* expression();
};

#endif