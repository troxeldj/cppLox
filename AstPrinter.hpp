#ifndef __AST_PRINT_HPP
#define __AST_PRINT_HPP
#include <iostream>
#include <vector>
#include "ExprVisitor.hpp"

class Expr;
class Binary;
class Grouping;
class Unary;
class Literal;

class AstPrinter : public ExprVisitor {
  std::any visitBinary(Binary*) override;
  std::any visitGrouping(Grouping*) override;
  std::any visitLiteral(Literal*) override;
  std::any visitUnary(Unary*) override;
  std::string parenthesize(std::string name, std::vector<Expr*> exprs);
public:
  AstPrinter() = default;
  std::string print(Expr*);
};

#endif