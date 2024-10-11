#include "Token.hpp"

Token::Token(TokenType type, std::string& lexeme, std::any literal, int line) {
  this->type = type;
  this->lexeme = lexeme;
  this->literal = std::move(literal);
  this->line = line;
}
Token::Token(TokenType type, const std::string& lexeme, std::any literal, int line) {
  this->type = type;
  this->lexeme = lexeme;
  this->literal = std::move(literal);
  this->line = line;
}

Token::Token(const Token& token) {
  this->type = token.type;
  this->lexeme = token.lexeme;
  this->literal = std::move(token.literal);
  this->line = token.line;
}

std::string Token::toString() {
  return tokenTypeToString(this->type) + " " + lexeme + " " + std::any_cast<std::string>(literal);
}