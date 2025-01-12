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

std::vector<Token>& Scanner::scanTokens() {
  while(!isAtEnd()) {
    start = current; 
    scanToken();
  }
  tokens.emplace_back(TokenType::END_OF_FILE, "", nullptr, line);
  return tokens;
}

char Scanner::consume() {
  if (isAtEnd()) return '\0';
  return content[current++];
}

char Scanner::peek() {
  if (isAtEnd()) return '\0';
  return content[current];
}

bool Scanner::match(char expected) {
  if(isAtEnd()) return false;
  if(content[current] != expected) return false;
  current++;
  return true;
}

void Scanner::scanToken() {
  char c = consume(); 
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
      addToken(match('=') ? TokenType::GREATER_EQUAL: TokenType::GREATER);
      break;
    case '/':
      if(match('/')) {
        while(peek() != '\n' && !isAtEnd()) consume();
      } else {
        addToken(TokenType::SLASH);
      }
      break;

    case ' ':
    case '\t':
    case '\r':
      break;

    case '\n':
      line++;
      break;

    case '"': string(); break;

   default:
    if(isDigit(c)) {
      number();
    } else if(isAlpha(c)) {
      identifier();
    } else {
      Lox::error(line, "Unexpected character.");
    }
  }
}

bool Scanner::isAtEnd() {
  return current >= content.length();
}

void Scanner::addToken(TokenType type) {
  tokens.emplace_back(type, content.substr(start, current - start), nullptr, line);
}

bool Scanner::isDigit(char c) {
  return c >= '0' && c <= '9';
}

bool Scanner::isAlpha(char c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

char Scanner::peekNext() {
  if(current + 1 >= content.length()) return '\0';
  return content[current + 1];
}

void Scanner::string() {
  while(peek() != '"' && !isAtEnd()) {
    if(peek() == '\n') line++;
    consume();
  }

  if(isAtEnd()) {
    Lox::error(line, "Unterminated string.");
    return;
  }
  // The closing paren
  consume();

  std::string value = content.substr(start + 1, current - 2 - start);
  tokens.emplace_back(TokenType::STRING, content.substr(start, current - start), value, line);
}

void Scanner::identifier() {
  while(isAlphaNumeric(peek())) consume();

  std::string text = content.substr(start, current - start);
  TokenType type;
  auto match = keywords.find(text);
  if(match == keywords.end()) {
    type = TokenType::IDENTIFIER;
  } else {
    type = match->second;
  }
  addToken(type);
}

void Scanner::number() {
  while(isDigit(peek())) consume();
  if(peek() == '.' && isDigit(peekNext())) {
    consume();
    while(isDigit(peek())) consume();
  }
  tokens.emplace_back(TokenType::NUMBER, content.substr(start, current - start), std::stod(content.substr(start, current - start)), line);
}

bool Scanner::isAlphaNumeric(char c) {
  return isAlpha(c) || isDigit(c);
}
