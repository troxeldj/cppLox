#include "Token.hpp"

std::string Token::toString() {
  return "Line " + std::to_string(line) + " " + ::toString(type) + " " + lexeme + "\n";
}