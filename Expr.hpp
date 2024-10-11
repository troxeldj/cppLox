#ifndef __EXPR_HPP
#define __EXPR_HPP
#include <any>

class ExprVisitor;

class Expr {
public:
  virtual std::any accept(ExprVisitor*) = 0;
  Expr() = default;
};

#endif