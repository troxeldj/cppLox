#include "AstPrinter.hpp"
#include "Expr.hpp"
#include "Binary.hpp"
#include "Literal.hpp"
#include "Unary.hpp"
#include "Grouping.hpp"
#include "util.hpp"
std::any AstPrinter::visitBinary(Binary* expr) {
  return parenthesize(expr->operate.lexeme, {expr->left, expr->right});
}

std::any AstPrinter::visitGrouping(Grouping* expr) {
  return parenthesize("group", {expr->expression});
}

std::any AstPrinter::visitLiteral(Literal* expr) {
  if(!expr->value.has_value()) return "nil";
  return expr->value;
}

std::any AstPrinter::visitUnary(Unary* expr) {
  return parenthesize(expr->operate.lexeme, {expr->right});
}

std::string AstPrinter::print(Expr* expr) {
  std::any ret = expr->accept(this);
  std::cout << convertAnyToString(ret) << std::endl;
  return convertAnyToString(ret);

}

std::string AstPrinter::parenthesize(std::string name, std::vector<Expr*> exprs) {
  std::string retStr = ""; 

  retStr += "(";
  retStr += name;
  for(Expr* expr : exprs) {
    retStr.append(" ");
    std::any exprVal = expr->accept(this);
    retStr.append(convertAnyToString(exprVal));
  }
  retStr.append(")");

  return retStr;
}