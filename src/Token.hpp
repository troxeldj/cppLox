#ifndef __TOKEN_HPP
#define __TOKEN_HPP
#include "TokenType.hpp"
#include <any>

struct Token {
  const TokenType type;
  const std::string lexeme;
  const std::any literal;
  const int line;

  Token(TokenType type, std::string lexeme, std::any literal, int line)
    : type(type), lexeme(lexeme), literal(std::move(literal)),line(line) {}
  Token(TokenType type, std::string lexeme, int line)
    : type(type), lexeme(lexeme), literal(nullptr), line(line) {}
  Token(Token& other)
    : type(other.type), lexeme(other.lexeme), literal(other.literal), line(other.line) {}
  Token(Token&& other) 
    : type(other.type), lexeme(other.lexeme), literal(std::move(other.literal)), line(other.line) {}
  Token(const Token& other) 
    : type(other.type), lexeme(other.lexeme), literal(other.literal), line(other.line) {}
  std::string toString();
};


#endif