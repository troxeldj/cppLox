#ifndef __EXPR_VIS_HPP
#define __EXPR_VIS_HPP
#include <any>

class Binary;
class Grouping;
class Literal;
class Unary;

class ExprVisitor {
public:
  virtual std::any visitBinary(Binary*) = 0;
  virtual std::any visitGrouping(Grouping*) = 0;
  virtual std::any visitLiteral(Literal*) = 0;
  virtual std::any visitUnary(Unary*) = 0;
};

#endif