#ifndef __BINARY_HPP
#define __BINARY_HPP
#include "Expr.hpp"
#include "Token.hpp"

class ExprVisitor;

class Binary : public Expr {
public:
  Expr* left;
  Token& operate;
  Expr* right;

  std::any accept(ExprVisitor*) override;

  Binary(Expr*, Token&, Expr*);
  Binary(Expr*, Token, Expr*);
};


#endif