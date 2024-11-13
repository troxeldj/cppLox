#pragma once
#include <any>
#include "TokenType.hpp"

struct Token {
  const TokenType type;
  const std::string lexeme;
  const std::any literal;
  const int line;

  // Constructors
  Token() = default;
  Token(TokenType type, std::string lexeme, std::any literal, int line)
    : type(std::move(type)), lexeme(std::move(lexeme)), literal(std::move(literal)),
    line(line) {}

  Token(const Token& other)
    : type(std::move(other.type)), lexeme(std::move(other.lexeme)), literal(std::move(other.literal)),
    line(other.line) {}

  Token(Token&& other)
    : type(std::move(other.type)), lexeme(std::move(other.lexeme)), literal(std::move(other.literal)),
    line(other.line) {}

  // Helpers
  std::string toString() const;
};
