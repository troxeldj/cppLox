#pragma once
#include <any>
#include <memory>
#include "Token.hpp"
struct Binary;
struct Grouping;
struct Literal;
struct Logical;
struct Unary;
struct Variable;
struct Assign;

struct ExprVisitor {
  virtual std::any visitBinaryExpr(std::shared_ptr<Binary> expr) = 0;
  virtual std::any visitGroupingExpr(std::shared_ptr<Grouping> expr) = 0;
  virtual std::any visitLiteralExpr(std::shared_ptr<Literal> expr) = 0;
  virtual std::any visitUnaryExpr(std::shared_ptr<Unary> expr) = 0;
  virtual std::any visitVariableExpr(std::shared_ptr<Variable> expr) = 0;
  virtual std::any visitAssignExpr(std::shared_ptr<Assign> expr) = 0;
  virtual std::any visitLogicalExpr(std::shared_ptr<Logical> expr) = 0;
  virtual ~ExprVisitor() = default;
};

struct Expr {
  virtual std::any accept(ExprVisitor& visitor) = 0;
};

struct Binary : public Expr, public std::enable_shared_from_this<Binary> {
  const std::shared_ptr<Expr> left;
  const Token op;
  const std::shared_ptr<Expr> right;

  Binary(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
    : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

  std::any accept(ExprVisitor& visitor) {
    return visitor.visitBinaryExpr(shared_from_this());
  }
};

struct Grouping : public Expr, public std::enable_shared_from_this<Grouping> {
  const std::shared_ptr<Expr> expression;

  Grouping(std::shared_ptr<Expr> expression) : expression(std::move(expression)) {}

  std::any accept(ExprVisitor& visitor) {
    return visitor.visitGroupingExpr(shared_from_this());
  }
};

struct Literal : public Expr, std::enable_shared_from_this<Literal> {
  const std::any value;

  Literal(std::any value) : value(std::move(value)) {}

  std::any accept(ExprVisitor& visitor) {
    return visitor.visitLiteralExpr(shared_from_this());
  }
};

struct Unary : public Expr, public std::enable_shared_from_this<Unary> {
  const std::shared_ptr<Expr> right;
  const Token op;

  Unary(Token op, std::shared_ptr<Expr> right)
    : op(std::move(op)), right(std::move(right)) {}

  std::any accept(ExprVisitor& visitor) {
    return visitor.visitUnaryExpr(shared_from_this());
  }
};

struct Variable : public Expr, public std::enable_shared_from_this<Variable> {
  const Token name;

  Variable(Token name) : name(std::move(name)) {}

  std::any accept(ExprVisitor& visitor) override {
    return visitor.visitVariableExpr(shared_from_this());
  }
};

struct Assign : public Expr, public std::enable_shared_from_this<Assign> {
  const Token name;
  const std::shared_ptr<Expr> value;

  Assign(Token name, std::shared_ptr<Expr> value)
    : name(std::move(name)), value(std::move(value)) {}

  std::any accept(ExprVisitor& visitor) override {
    return visitor.visitAssignExpr(shared_from_this());
  }
};

struct Logical : public Expr, public std::enable_shared_from_this<Logical> {

  Logical(std::shared_ptr<Expr> left, Token op, std::shared_ptr<Expr> right)
    : left(std::move(left)), op(std::move(op)), right(std::move(right)) {}

  std::any accept(ExprVisitor& visitor) override {
    return visitor.visitLogicalExpr(shared_from_this());
  }

  const std::shared_ptr<Expr> left;
  const Token op;
  const std::shared_ptr<Expr> right;
};