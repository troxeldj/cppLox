#include "Token.hpp"

Token::Token(TokenType type, const std::string& lexeme, const std::string& literal, int line) {
  this->type = type;
  this->lexeme = lexeme;
  this->literal = literal;
  this->line = line;
}

Token::Token(const Token& token) {
  this->type = token.type;
  this->lexeme = token.lexeme;
  this->literal = token.literal;
  this->line = token.line;
}

std::string Token::toString() {
  return tokenTypeToString(this->type) + " " + lexeme + " " + literal;
}