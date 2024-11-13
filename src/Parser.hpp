#include <stdexcept>
#include <vector>
#include <cassert>
#include "Lox.hpp"
#include "Expr.hpp"
class Parser {
  struct ParseError : public std::runtime_error {
    using std::runtime_error::runtime_error;
  };

  const std::vector<Token>& tokens;
  int current;
public:
  Parser(const std::vector<Token>& tokens)
    : tokens(tokens), current(0) {}

  std::shared_ptr<Expr> parse() {
    try {
      return expression();
    } catch (ParseError error) {
      return nullptr;
    }
  }

private:
  std::shared_ptr<Expr> expression() {
    return equality();
  }

  std::shared_ptr<Expr> equality() {
    std::shared_ptr<Expr> expr = comparison();
    
    while(match(TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL)) {
      Token op = previous();
      std::shared_ptr<Expr> right = comparison();
      expr = std::make_shared<Binary>(expr, std::move(op), right);
    }
    return expr;
  }

  std::shared_ptr<Expr> comparison() {
    std::shared_ptr<Expr> expr = term();

    while(match(TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL)) {
      Token op = previous();
      std::shared_ptr<Expr> right = term();
      expr = std::make_shared<Binary>(expr, std::move(op), right);
    }
    return expr;
  }

  std::shared_ptr<Expr> term() {
    std::shared_ptr<Expr> expr = factor();

    while(match(TokenType::MINUS, TokenType::PLUS)) {
      Token op = previous();
      std::shared_ptr<Expr> right = factor();
      expr = std::make_shared<Binary>(expr, std::move(op), right);
    }
    return expr;
  }

  std::shared_ptr<Expr> factor() {
    std::shared_ptr<Expr> expr = unary();

    while(match(TokenType::SLASH, TokenType::STAR)) {
      Token op = previous();
      std::shared_ptr<Expr> right = unary();
      expr = std::make_shared<Binary>(expr, std::move(op), right);
    }
    return expr;
  }

  std::shared_ptr<Expr> unary() {
    if(match(TokenType::BANG, TokenType::MINUS)) {
      Token op = previous();
      std::shared_ptr<Expr> right = unary();
      return std::make_shared<Unary>(std::move(op), right);
    }
    return primary();
  }

  std::shared_ptr<Expr> primary() {
    if(match(TokenType::FALSE)) return std::make_shared<Literal>(false);
    if(match(TokenType::TRUE)) return std::make_shared<Literal>(true);
    if(match(TokenType::NIL)) return std::make_shared<Literal>(nullptr);

    if(match(TokenType::NUMBER, TokenType::STRING)) {
      return std::make_shared<Literal>(previous().literal);
    }

    if(match(TokenType::LEFT_PAREN)) {
      std::shared_ptr<Expr> expr = expression();
      consume(TokenType::RIGHT_PAREN, "Expect ')' after expression");
      return std::make_shared<Grouping>(expr);
    }

    throw error(peek(), "Expect expression.");
  }

  template <class... T>
  bool match(T... type) {
    assert((... && std::is_same_v<T, TokenType>));

    if((... || check(type))) {
      advance();
      return true;
    }
    return false;
  }

  Token consume(TokenType type, std::string message) {
    if(check(type)) return advance();

    throw error(peek(), message);
  }

  bool check(TokenType type) {
    if(isAtEnd()) return false;
    return peek().type == type;
  }

  Token advance() {
    if(!isAtEnd()) ++current;
    return previous();
  }

  bool isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
  }

  Token peek() {
    return tokens.at(current);
  }

  Token previous() {
    return tokens.at(current - 1);
  }

  ParseError error(const Token& token, std::string message) {
    Lox::error(token, message);
    return ParseError{""};
  }

  void syncronize() {
    advance();
    while(!isAtEnd()) {
      if(previous().type == TokenType::SEMICOLON) return;

      switch(peek().type) {
        case TokenType::CLASS:
        case TokenType::FUN:
        case TokenType::VAR:
        case TokenType::FOR:
        case TokenType::IF:
        case TokenType::WHILE:
        case TokenType::PRINT:
        case TokenType::RETURN:
          return;
      }

      advance();
    }
  }
};