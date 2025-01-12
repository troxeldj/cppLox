#include <vector>
#include <iostream>
#include <memory>
#include "TokenType.hpp"

struct Token;
struct Expr;

class ParseError : public std::runtime_error {
public:
  ParseError() : std::runtime_error("Parse error.") {}
  ParseError(const std::string& message) : std::runtime_error(message) {}
};

class Parser {
  std::vector<Token> tokens;
  int current;
private:
  ParseError error(Token token, const std::string& message);
  // Expression parsing
  std::shared_ptr<Expr> expression();
  std::shared_ptr<Expr> assignment();
  std::shared_ptr<Expr> equality();
  std::shared_ptr<Expr> comparison();
  std::shared_ptr<Expr> term();
  std::shared_ptr<Expr> orExpr();
  std::shared_ptr<Expr> andExpr();
  std::shared_ptr<Expr> factor();
  std::shared_ptr<Expr> unary();
  std::shared_ptr<Function> function(std::string kind);
  std::shared_ptr<Expr> call();
  std::shared_ptr<Expr> finishCall(std::shared_ptr<Expr> callee);
  std::shared_ptr<Expr> primary();

  // Statement parsing
  std::shared_ptr<Stmt> declaration();
  std::shared_ptr<Stmt> classDeclaration();
  std::shared_ptr<Stmt> statement();
  std::shared_ptr<Stmt> printStatement();
  std::shared_ptr<Stmt> expressionStatement();
  std::shared_ptr<Stmt> varDeclaration();
  std::shared_ptr<Stmt> ifStatement();
  std::shared_ptr<Stmt> whileStatement();
  std::shared_ptr<Stmt> forStatement();
  std::shared_ptr<Stmt> returnStatement();
  std::vector<std::shared_ptr<Stmt>> block();
  bool match(std::vector<TokenType> types);
  bool check(TokenType type);
  bool isAtEnd();
  Token advance();
  Token previous();
  Token peek();
  Token consume(TokenType type, std::string message);
  void syncronize();
public:
  Parser(std::vector<Token>& tokens) 
    : tokens{std::move(tokens)}, current{0} {}
  std::vector<std::shared_ptr<Stmt>> parse();
};