#ifndef __LITERAL_HPP
#define __LITERAL_HPP
#include <iostream>
#include "Expr.hpp"

class ExprVisitor;

class Literal : public Expr {
public:
  std::any value; 
  Literal(std::any);
  std::any accept(ExprVisitor*) override;

};

#endif