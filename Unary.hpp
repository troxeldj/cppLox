#ifndef __UNARY_HPP
#define __UNARY_HPP
#include "Token.hpp"
#include "Expr.hpp"

class ExprVisitor;

class Unary : public Expr {
public:
  Token& operate;
  Expr* right;
  Unary(Token&, Expr*);
  Unary(Token, Expr*);
  std::any accept(ExprVisitor*);
};

#endif