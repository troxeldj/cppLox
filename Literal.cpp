#include "Literal.hpp"
#include "ExprVisitor.hpp"

std::any Literal::accept(ExprVisitor* vis) {
  return vis->visitLiteral(this);
}

Literal::Literal(std::any value) : value(value) {}