#include "Binary.hpp"
#include "ExprVisitor.hpp"

Binary::Binary(Expr* left, Token& operate, Expr* right) : left(left), operate(operate), right(right) {} 
Binary::Binary(Expr* left, Token operate, Expr* right) : left(left), operate(operate), right(right) {}

std::any Binary::accept(ExprVisitor* vis) {
  return vis->visitBinary(this);
}