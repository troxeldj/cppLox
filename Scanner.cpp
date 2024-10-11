#include "Scanner.hpp"
#include "Lox.hpp"

std::map<std::string, TokenType> Scanner::keywords = {
  {"and", TokenType::AND},
  {"class", TokenType::CLASS},
  {"else", TokenType::ELSE},
  {"false", TokenType::FALSE},
  {"for", TokenType::FOR},
  {"fun", TokenType::FUN},
  {"if", TokenType::IF},
  {"nil", TokenType::NIL},
  {"or", TokenType::OR},
  {"print", TokenType::PRINT},
  {"return", TokenType::RETURN},
  {"super", TokenType::SUPER},
  {"this", TokenType::THIS},
  {"true", TokenType::TRUE},
  {"var", TokenType::VAR},
  {"while", TokenType::WHILE}
};

Scanner::Scanner(std::string& source) {
  this->source = source;
  this->tokens = std::vector<Token>();
  this->start = 0;
  this->current = 0;
  this->line = 1;
}

Scanner::~Scanner() {
  this->tokens.clear();
}

bool Scanner::match(char expected) {
  if(isAtEnd()) return false;
  if(source[current] != expected) return false;
  current++;
  return true;
}

bool Scanner::isAlpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Scanner::isDigit(char c) {
  return c >= '0' && c <= '9';
}

bool Scanner::isAlphaNumeric(char c) {
  return isAlpha(c) || isDigit(c);
}

char Scanner::peek() {
  return source[current];
}

bool Scanner::isAtEnd() {
  return current >= source.length();
}

char Scanner::advance() {
  return source[current++];
}

std::vector<Token>& Scanner::scanTokens() {
  while(!isAtEnd()) {
    start = current;
    scanToken();
  }
  tokens.push_back(Token(TokenType::__EOF, "", std::any(""), line));
  return tokens;
}

Token Scanner::scanToken() {
  char c = advance();
  switch(c) {
    case '(': addToken(TokenType::LEFT_PAREN); break;
    case ')': addToken(TokenType::RIGHT_PAREN); break;
    case '{': addToken(TokenType::LEFT_BRACE); break;
    case '}': addToken(TokenType::RIGHT_BRACE); break;
    case ',': addToken(TokenType::COMMA); break;
    case '.': addToken(TokenType::DOT); break;
    case '-': addToken(TokenType::MINUS); break;
    case '+': addToken(TokenType::PLUS); break;
    case ';': addToken(TokenType::SEMICOLON); break;
    case '*': addToken(TokenType::STAR); break;
    case '!': 
      addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG); 
      break;
    case '=': 
      addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL); 
      break;
    case '<': 
      addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
      break;
    case '>': 
      addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
      break;
    case '/':
      if(match('/')) {
        while(peek() != '\n' && !isAtEnd()) advance();
      } else {
        addToken(TokenType::SLASH);
      }
      break;
    case ' ':
    case '\r':
    case '\t':
      break;
    case '\n':
      line++;
      break;
    case '"':
      string();
      break;
    case 'o':
      if(match('r')) addToken(TokenType::OR);
      break;
    default:
      if(isDigit(c)) {
        number();
      } else if (isAlpha(c)) {
        identifier();
      } else {
        Lox::error(line, "Unexpected character.");
      }
      break;
  }
}

void Scanner::addToken(TokenType type) {
  std::string value = "";
  addToken(type, value);
}

void Scanner::addToken(TokenType type, std::any value) {
  tokens.emplace_back(type, source.substr(start, current - start), value, line);
}

void Scanner::string() {
  while(peek() != '"' && !isAtEnd()) {
    if(peek() == '\n') line++;
    advance();
  }

  if(isAtEnd()) {
    Lox::error(line, "Unterminated string.");
    return;
  }

  advance();
  std::string value = source.substr(start + 1, current - start - 2);
  addToken(TokenType::STRING, value);
}

void Scanner::number() {
  while(isDigit(peek())) advance();

  if(peek() == '.' && isDigit(peek())) {
    advance();
    while(isDigit(peek())) advance();
  }

  std::string value = source.substr(start, current - start);
  addToken(TokenType::NUMBER, value);
}

void Scanner::identifier() {
  while(isAlphaNumeric(peek())) advance();
  std::string value = source.substr(start, current - start);
  if(keywords.find(value) != keywords.end()) {
    addToken(keywords[value], value);
    return;
  } else {
    addToken(TokenType::IDENTIFIER, value);
  }
}