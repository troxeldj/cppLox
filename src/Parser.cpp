#include "Lox.hpp"
#include "Parser.hpp"
#include "TokenType.hpp"
#include "Token.hpp"
#include "Expr.hpp"

std::shared_ptr<Expr> Parser::expression() {
  return assignment();
}

std::shared_ptr<Expr> Parser::assignment() {
  std::shared_ptr<Expr> expr = orExpr();
  if (match({TokenType::EQUAL})) {
    Token equals = previous();
    std::shared_ptr<Expr> value = assignment();
    if(Variable* e = dynamic_cast<Variable*>(expr.get())) {
      const Token& name = e->name;
      return std::make_shared<Assign>(std::move(name), value);
    } else if(Get* e = dynamic_cast<Get*>(expr.get())) {
      return std::make_shared<Set>(e->object, e->name, value);
    }
    error(std::move(equals), "Invalid assignment target.");
  }
  return expr;
}

std::shared_ptr<Expr> Parser::orExpr() {
  std::shared_ptr<Expr> expr = andExpr();
  while (match({TokenType::OR})) {
    Token oper = previous();
    std::shared_ptr<Expr> right = andExpr();
    expr = std::make_shared<Logical>(expr, std::move(oper), right);
  }
  return expr;
}

std::shared_ptr<Expr> Parser::andExpr() {
  std::shared_ptr<Expr> expr = equality();
  while (match({TokenType::AND})) {
    Token oper = previous();
    std::shared_ptr<Expr> right = equality();
    expr = std::make_shared<Logical>(expr, std::move(oper), right);
  }
  return expr;
}

std::shared_ptr<Expr> Parser::equality() {
  std::shared_ptr<Expr> expr = comparison();
  while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
    Token oper = previous();
    std::shared_ptr<Expr> right = comparison();
    expr = std::make_shared<Binary>(expr, std::move(oper), right);
  }
  return expr;
}

std::shared_ptr<Expr> Parser::comparison() {
  std::shared_ptr<Expr> expr = term();
  while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})) {
    Token oper = previous();
    std::shared_ptr<Expr> right = term();
    expr = std::make_shared<Binary>(expr, std::move(oper), right);
  }
  return expr;
}

std::shared_ptr<Expr> Parser::term() {
  std::shared_ptr<Expr> expr = factor();
  while (match({TokenType::MINUS, TokenType::PLUS})) {
    Token oper = previous();
    std::shared_ptr<Expr> right = factor();
    expr = std::make_shared<Binary>(expr, std::move(oper), right);
  }
  return expr;
}

std::shared_ptr<Expr> Parser::factor() {
  std::shared_ptr<Expr> expr = unary();
  while (match({TokenType::SLASH, TokenType::STAR})) {
    Token oper = previous();
    std::shared_ptr<Expr> right = unary();
    expr = std::make_shared<Binary>(expr, std::move(oper), right);
  }
  return expr;
}

std::shared_ptr<Expr> Parser::unary() {
  if (match({TokenType::BANG, TokenType::MINUS})) {
    Token oper = previous();
    std::shared_ptr<Expr> right = unary();
    return std::make_shared<Unary>(std::move(oper), right);
  }
  return call();
}

std::shared_ptr<Expr> Parser::call() {
  std::shared_ptr<Expr> expr = primary();
  while (true) {
    if (match({TokenType::LEFT_PAREN})) {
      expr = finishCall(expr);
    } else if (match({TokenType::DOT})) {
      Token name = consume(TokenType::IDENTIFIER, "Expect property name after '.'.");
      expr = std::make_shared<Get>(expr, std::move(name));
    } else {
      break;
    }
  }
  return expr;
}

std::shared_ptr<Expr> Parser::finishCall(std::shared_ptr<Expr> callee) {
  std::vector<std::shared_ptr<Expr>> arguments;
  if (!check(TokenType::RIGHT_PAREN)) {
    do {
      if (arguments.size() >= 255) {
        error(peek(), "Cannot have more than 255 arguments.");
      }
      arguments.push_back(expression());
    } while (match({TokenType::COMMA}));
  }
  Token paren = consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
  return std::make_shared<Call>(callee, std::move(paren), std::move(arguments));
}

std::shared_ptr<Expr> Parser::primary() {
  if (match({TokenType::FALSE})) return std::make_shared<Literal>(false);
  if (match({TokenType::TRUE})) return std::make_shared<Literal>(true);
  if (match({TokenType::NIL})) return std::make_shared<Literal>(nullptr);

  if (match({TokenType::NUMBER, TokenType::STRING})) {
    return std::make_shared<Literal>(previous().literal);
  }

  if(match({TokenType::SUPER})) {
    Token keyword = previous();
    consume(TokenType::DOT, "Expect '.' after 'super'.");
    Token method = consume(TokenType::IDENTIFIER, "Expect superclass method name.");
    return std::make_shared<Super>(keyword, method);
  }

  if(match({TokenType::THIS})) return std::make_shared<This>(previous());

  if(match({TokenType::IDENTIFIER})) {
    return std::make_shared<Variable>(previous());
  }

  if (match({TokenType::LEFT_PAREN})) {
    std::shared_ptr<Expr> expr = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
    return std::make_shared<Grouping>(std::move(expr));
  }

  throw error(peek(), "Expect expression.");
}

std::shared_ptr<Stmt> Parser::statement() {
  if(match({TokenType::FOR})) return forStatement();
  if(match({TokenType::IF})) return ifStatement();
  if(match({TokenType::PRINT})) return printStatement();
  if(match({TokenType::RETURN})) return returnStatement();
  if(match({TokenType::WHILE})) return whileStatement();
  if(match({TokenType::LEFT_BRACE})) return std::make_shared<Block>(block());
  return expressionStatement();
}

std::shared_ptr<Stmt> Parser::returnStatement() {
  Token keyword = previous();
  std::shared_ptr<Expr> value = nullptr;
  if(!check(TokenType::SEMICOLON)) {
    value = expression();
  }
  consume(TokenType::SEMICOLON, "Expect ';' after return value.");
  return std::make_shared<Return>(keyword, value);
}

std::shared_ptr<Stmt> Parser::ifStatement() {
  consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
  std::shared_ptr<Expr> condition = expression();
  consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");

  std::shared_ptr<Stmt> thenBranch = statement();
  std::shared_ptr<Stmt> elseBranch = nullptr;
  if(match({TokenType::ELSE})) {
    elseBranch = statement();
  }
  return std::make_shared<If>(std::move(condition), std::move(thenBranch), std::move(elseBranch));
}

std::vector<std::shared_ptr<Stmt>> Parser::block() {
  std::vector<std::shared_ptr<Stmt>> statements;
  while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
    statements.push_back(declaration());
  }
  consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
  return statements;
}

std::shared_ptr<Stmt> Parser::printStatement() {
  std::shared_ptr<Expr> value = expression();
  consume(TokenType::SEMICOLON, "Expect ';' after value.");
  return std::make_shared<Print>(value);
}

std::shared_ptr<Stmt> Parser::whileStatement() {
  consume(TokenType::LEFT_PAREN, "Expect '(' after 'while'.");
  std::shared_ptr<Expr> condition = expression();
  consume(TokenType::RIGHT_PAREN, "Expect ')' after condition.");
  std::shared_ptr<Stmt> body = statement();
  return std::make_shared<While>(condition, body);
}

std::shared_ptr<Stmt> Parser::forStatement() {
  consume(TokenType::LEFT_PAREN, "Expect '(' after 'for'.");
  std::shared_ptr<Stmt> initializer;
  if(match({TokenType::SEMICOLON})) {
    initializer = nullptr;
  } else if(match({TokenType::VAR})) {
    initializer = varDeclaration();
  } else {
    initializer = expressionStatement();
  }
  std::shared_ptr<Expr> condition = nullptr;
  if(!check(TokenType::SEMICOLON)) {
    condition = expression();
  }
  consume(TokenType::SEMICOLON, "Expect ';' after loop condition.");
  std::shared_ptr<Expr> increment = nullptr;
  if(!check(TokenType::RIGHT_PAREN)) {
    increment = expression();
  }
  consume(TokenType::RIGHT_PAREN, "Expect ')' after for clauses.");
  std::shared_ptr<Stmt> body = statement();
  if(increment != nullptr) {
    body = std::make_shared<Block>(std::vector<std::shared_ptr<Stmt>>{
      body,
      std::make_shared<Expression>(std::move(increment))
    });
  }
  if(condition == nullptr) condition = std::make_shared<Literal>(true);
  body = std::make_shared<While>(condition, body);
  if(initializer != nullptr) {
    body = std::make_shared<Block>(std::vector<std::shared_ptr<Stmt>>{
      initializer,
      body
    });
  }
  return body;
}

std::shared_ptr<Stmt> Parser::declaration() {
  try {
    if(match({TokenType::CLASS})) return classDeclaration();
    if(match({TokenType::FUN})) return function("function");
    if(match({TokenType::VAR})) return varDeclaration();
    return statement();
  } catch(ParseError& error) {
    syncronize();
    return nullptr;
  }
}

std::shared_ptr<Stmt> Parser::classDeclaration() {
  Token name = consume(TokenType::IDENTIFIER, "Expect class name.");
  std::shared_ptr<Variable> superclass = nullptr;
  if(match({TokenType::LESS})) {
    consume(TokenType::IDENTIFIER, "Expect superclass name.");
    superclass = std::make_shared<Variable>(previous());
  }

  consume(TokenType::LEFT_BRACE, "Expect '{' before class body.");
  std::vector<std::shared_ptr<Function>> methods;
  while(!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
    methods.push_back(function("method"));
  }
  consume(TokenType::RIGHT_BRACE, "Expect '}' after class body.");
  return std::make_shared<Class>(std::move(name), superclass, std::move(methods));
}

std::shared_ptr<Function> Parser::function(std::string kind) {
  Token name = consume(TokenType::IDENTIFIER, "Expect " + kind + " name.");
  consume(TokenType::LEFT_PAREN, "Expect '(' after " + kind + " name.");
  std::vector<Token> params;
  if (!check(TokenType::RIGHT_PAREN)) {
    do {
      if (params.size() >= 255) {
        error(peek(), "Cannot have more than 255 parameters.");
      }
      params.push_back(consume(TokenType::IDENTIFIER, "Expect parameter name."));
    } while (match({TokenType::COMMA}));
  }
  consume(TokenType::RIGHT_PAREN, "Expect ')' after parameters.");
  consume(TokenType::LEFT_BRACE, "Expect '{' before " + kind + " body.");
  std::vector<std::shared_ptr<Stmt>> body = block();
  return std::make_shared<Function>(std::move(name), std::move(params), std::move(body));
}

std::shared_ptr<Stmt> Parser::varDeclaration() {
  Token name = consume(TokenType::IDENTIFIER, "Expect variable name.");
  std::shared_ptr<Expr> initializer = nullptr;
  if(match({TokenType::EQUAL})) {
    initializer = expression();
  }
  consume(TokenType::SEMICOLON, "Expect ';' after variable declaration.");
  return std::make_shared<Var>(name, std::move(initializer));
}

std::shared_ptr<Stmt> Parser::expressionStatement() {
  std::shared_ptr<Expr> expr = expression();
  consume(TokenType::SEMICOLON, "Expect ';' after expression.");
  return std::make_shared<Expression>(expr);
}

bool Parser::match(std::vector<TokenType> types) {
  for (int i = 0; i < types.size(); i++) {
    if (check(types.at(i))) {
      advance();
      return true;
    }
  }
  return false;
}

bool Parser::isAtEnd() {
  return peek().type == TokenType::END_OF_FILE;
}

bool Parser::check(TokenType type) {
  if (isAtEnd()) return false;
  return peek().type == type;
}

Token Parser::advance() {
  if (!isAtEnd()) current++;
  return previous();
}

Token Parser::peek() {
  return tokens.at(current);
}

Token Parser::previous() {
  return tokens.at(current - 1);
}

Token Parser::consume(TokenType type, std::string message) {
  if (check(type)) return advance();
  throw error(peek(), message);
}

ParseError Parser::error(Token token, const std::string& message) {
  Lox::error(token, message);
  return ParseError(message);
}

void Parser::syncronize() {
  advance();
  while (!isAtEnd()) {
    if (previous().type == TokenType::SEMICOLON) return;
    switch (peek().type) {
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

std::vector<std::shared_ptr<Stmt>> Parser::parse() {
  std::vector<std::shared_ptr<Stmt>> statements;
  while (!isAtEnd()) {
    statements.push_back(declaration());
  }
  return statements;
}
