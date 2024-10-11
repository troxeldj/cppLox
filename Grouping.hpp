#ifndef __GROUPING_HPP
#define __GROUPING_HPP
#include "Expr.hpp"

class ExprVisitor;

class Grouping : public Expr {
public:
  Expr* expression;
  Grouping(Expr*);
  std::any accept(ExprVisitor*) override;
};

#endif
