#include <stdexcept>
#include <vector>
#include <cassert>
#include "Lox.hpp"
#include "Stmt.hpp"

class Parser {
  struct ParseError : public std::runtime_error {
    using std::runtime_error::runtime_error;
  };

  const std::vector<Token>& tokens;
  int current;
public:
  Parser(const std::vector<Token>& tokens)
    : tokens(tokens), current(0) {}

  std::vector<std::shared_ptr<Stmt>> parse() {
    std::vector<std::shared_ptr<Stmt>> statements;
    while(!isAtEnd()) {
      statements.push_back(declaration());
    }
    return statements;
  }

private:
  std::shared_ptr<Expr> expression() {
    return assignment();
  }

  std::shared_ptr<Stmt> declaration() {
    try {
      if(match(TokenType::VAR)) return varDeclaration();
      return statement();
    } catch (ParseError error) {
      syncronize();
      return nullptr;
    }
  }

  std::shared_ptr<Stmt> statement() {
    if(match(TokenType::FOR)) return forStatement();
    if(match(TokenType::IF)) return ifStatement();
    if(match(TokenType::WHILE)) return whileStatement();
    if(match(TokenType::PRINT)) return printStatement();
    if(match(TokenType::LEFT_BRACE)) return std::make_shared<Block>(block());
    return expressionStatement();
  }

  std::shared_ptr<Stmt> forStatement() {
    consume(TokenType::LEFT_PAREN, "Expected ')' after 'for'.");

    std::shared_ptr<Stmt> initializer;
    if(match(TokenType::SEMICOLON)) initializer = nullptr;
    else if (match(TokenType::VAR)) initializer = varDeclaration();
    else initializer = expressionStatement();

    std::shared_ptr<Expr> condition = nullptr;
    if(!check(TokenType::SEMICOLON)) {
      condition = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");

    std::shared_ptr<Expr> increment = nullptr;
    if(!check(TokenType::RIGHT_PAREN)) {
      increment = expression();
    }
    consume(TokenType::RIGHT_PAREN, "Expected ';' after for clauses");
    std::shared_ptr<Stmt> body = statement();
    if(increment != nullptr) {
      body = std::make_shared<Block>(
        std::vector<std::shared_ptr<Stmt>>{body, std::make_shared<Expression>(increment)}
      );
    }
    if(condition == nullptr) {
      condition = std::make_shared<Literal>(true);
    }
    body = std::make_shared<While>(condition, body);
    
    if(initializer != nullptr) {
      body = std::make_shared<Block>(
        std::vector<std::shared_ptr<Stmt>>{initializer, body}
      );
    }
    return body;
  }

  std::shared_ptr<Stmt> ifStatement() {
    consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
    std::shared_ptr<Expr> condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after if condition.");

    std::shared_ptr<Stmt> thenBranch = statement();
    std::shared_ptr<Stmt> elseBranch = nullptr;
    if(match(TokenType::ELSE)) {
      elseBranch = statement();
    }
    return std::make_shared<If>(condition, thenBranch, elseBranch);
  }

  std::shared_ptr<Stmt> whileStatement() {
    consume(TokenType::LEFT_PAREN, "Expected '(' after 'while'.");
    std::shared_ptr<Expr> condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after condition.");
    std::shared_ptr<Stmt> body = statement();
    return std::make_shared<While>(condition, body);
  }

  std::shared_ptr<Expr> orExpression() {
    std::shared_ptr<Expr> expr = andExpression();

    while(match(TokenType::OR)) {
      Token op = previous();
      std::shared_ptr<Expr> right = equality();
      expr = std::make_shared<Logical>(expr, std::move(op), right);
    }

    return expr;
  }

  std::shared_ptr<Expr> andExpression() {
    std::shared_ptr<Expr> expr = equality();

    while(match(TokenType::AND)) {
      Token op = previous();
      std::shared_ptr<Expr> right = equality();
      expr = std::make_shared<Logical>(expr, std::move(op), right);
    }
    return expr;
  }

  std::shared_ptr<Stmt> printStatement() {
    std::shared_ptr<Expr> value = expression();
    consume(TokenType::SEMICOLON, "Expect ; after value.");
    return std::make_shared<Print>(value);
  }

  std::shared_ptr<Stmt> varDeclaration() {
    Token name = consume(TokenType::IDENTIFIER, "Expect variable name");

    std::shared_ptr<Expr> initilizer = nullptr;
    if(match(TokenType::EQUAL)) {
      initilizer = expression();
    }
    consume(TokenType::SEMICOLON, "Expect ';' after variable definition.");
    return std::make_shared<Var>(std::move(name), initilizer);
  }

  std::shared_ptr<Stmt> expressionStatement() {
    std::shared_ptr<Expr> expr = expression();
    consume(TokenType::SEMICOLON, "Expect ';' after expression.");
    return std::make_shared<Expression>(expr);
  }

  std::vector<std::shared_ptr<Stmt>> block() {
    std::vector<std::shared_ptr<Stmt>> statements;

    while(!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
      statements.push_back(declaration());
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
    return statements;
  }

  std::shared_ptr<Expr> assignment() {
    std::shared_ptr<Expr> expr = orExpression();

    if(match(TokenType::EQUAL)) {
      Token equals = previous();
      std::shared_ptr<Expr> value = assignment();

      if(Variable* e = dynamic_cast<Variable*>(expr.get())) {
        Token name = e->name;
        return std::make_shared<Assign>(std::move(name), value);
      }

      Lox::error(std::move(equals), "Invalid assignment target.");
    }
    return expr;
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

    if(match(TokenType::IDENTIFIER)) {
      return std::make_shared<Variable>(previous());
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