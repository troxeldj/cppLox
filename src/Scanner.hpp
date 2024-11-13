#pragma once
#include "Token.hpp"
#include "Util.hpp"
#include <vector>

struct Scanner {
  const std::string source;
  int start;
  int current;
  int line;
  std::vector<Token> tokens;
  // Constructors
  Scanner(std::string source) 
    : source(std::move(source)), start(0), current(0), line(1) {}

  // Copy
  Scanner(const Scanner& other)
    : source(std::move(other.source)), start(other.start), 
      current(other.current), line(other.line) {}

  // Move 
  Scanner(Scanner&& other)
    : source(std::move(other.source)), start(other.start), 
      current(other.current), line(other.line) {}

  // Helpers
  bool isAtEnd();
  bool match(char);

  void number();
  void string();
  void Identifier();

  std::vector<Token> scanTokens();
  void scanToken();

  char advance();
  char peek();
  char peekNext();

  bool isDigit(char);
  bool isAlpha(char);
  bool isAlphaNumeric(char);

  void addToken(TokenType);
  void addToken(TokenType, std::any);
};