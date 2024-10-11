#include "Unary.hpp"
#include "ExprVisitor.hpp"

Unary::Unary(Token& operate, Expr* expr) : right(expr), operate(operate) {}
Unary::Unary(Token operate, Expr* expr) : right(expr), operate(operate) {}


std::any Unary::accept(ExprVisitor* vis) {
  return vis->visitUnary(this);
}

