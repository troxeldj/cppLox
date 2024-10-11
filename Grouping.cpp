#include "Grouping.hpp"
#include "ExprVisitor.hpp"

Grouping::Grouping(Expr* expression) : expression(expression) {};

std::any Grouping::accept(ExprVisitor* vis) {
  return vis->visitGrouping(this);
}