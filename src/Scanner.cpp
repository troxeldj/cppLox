#include "Scanner.hpp"
#include "Lox.hpp"
#include <string>

bool Scanner::isAtEnd() {
  return current >= source.length();
}

bool Scanner::match(char expected) {
  if(isAtEnd()) return false;
  if(source[current] != expected) return false;
  current++;
  return true;
}

void Scanner::number() {
  while(isDigit(peek())) advance();

  if(peek() == '.' && isDigit(peekNext())) {
    advance();

    while(isDigit(peek())) advance();
  }

  addToken(TokenType::NUMBER, std::stod(std::string(source.substr(start, current - start))));
}

void Scanner::string() {
  while(peek() != '"' && !isAtEnd()) {
    if(peek() == '\n') line++;
    advance();
  }

  if(isAtEnd()) {
    Lox::error(line, "Unterminated String.");
  }

  advance();

  std::string value = source.substr(start + 1, current - 2 - start);
  addToken(TokenType::STRING, value);
}

void Scanner::Identifier() {
  while(isAlphaNumeric(peek())) advance();

  std::string text = source.substr(start, current - start);

  TokenType type;
  auto match = Util::keywords.find(text);
  if(match == Util::keywords.end()) {
    type = TokenType::IDENTIFIER;
  } else {
    type = match->second;
  }

  addToken(type);
}

char Scanner::advance() {
  if(isAtEnd()) return '\0';
  return source[current++];
}

char Scanner::peek() {
  if(isAtEnd()) return '\0';
  return source[current];
}

char Scanner::peekNext() {
  if(current + 1 >= source.length()) return '\0';
  return source[current+1];
}

std::vector<Token> Scanner::scanTokens() {
  while(!isAtEnd()) {
    start = current;
    scanToken();
  }
  tokens.emplace_back(TokenType::END_OF_FILE, "", nullptr, line);
  return tokens;
}

void Scanner::scanToken() {
  char c = advance();

  switch(c) {
    case '(':
      addToken(TokenType::LEFT_PAREN);
      break;
    case ')':
      addToken(TokenType::RIGHT_PAREN);
      break;
    case '{':
      addToken(TokenType::LEFT_BRACE);
      break;
    case '}':
      addToken(TokenType::RIGHT_BRACE);
      break;
    case ',':
      addToken(TokenType::COMMA);
      break;
    case '.':
      addToken(TokenType::DOT);
      break;
    case '-':
      addToken(TokenType::MINUS);
      break;
    case '+':
      addToken(TokenType::PLUS);
      break;
    case ';':
      addToken(TokenType::SEMICOLON);
      break;
    case '*':
      addToken(TokenType::STAR);
      break;
    case '!':
      addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
      break;
    case '<':
      addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
      break;
    case '>':
      addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
      break;
    case '=':
      addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
      break;
    case '/':
      if(match('/')) {
        while(peek() != '\n' && !isAtEnd()) advance();
      } else {
        addToken(TokenType::SLASH);
      }
      break;
    // Whitespace
    case ' ':
    case '\r':
    case '\t':
      break;

    // Newline
    case '\n':
      line++;
      break;
    // string
    case '"':
      string();
      break;

    default:
      if(isDigit(c)) {
        number();
      } else if(isAlpha(c)) {
        Identifier();
      } else {
        Lox::error(line, "Unexpected Character.");
      }
      break;
  }
}

bool Scanner::isDigit(char c) {
  return c >= '0' && c <= '9';
}

bool Scanner::isAlpha(char c) {
  return (
    (c >= 'a' && c <= 'z') ||
    (c >= 'A' && c <= 'Z') ||
    (c == '_')
  );
}

bool Scanner::isAlphaNumeric(char c) {
  return isDigit(c) || isAlpha(c);
}

void Scanner::addToken(TokenType type) {
  addToken(type, nullptr);
}

void Scanner::addToken(TokenType type, std::any literal) {
  std::string text = source.substr(start, current - start);
  tokens.emplace_back(type, std::move(text), std::move(literal), line);
}