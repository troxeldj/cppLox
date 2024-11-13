#include "Expr.hpp"


class Interpreter : public ExprVisitor {
public:
  void interpret(std::shared_ptr<Expr> expression) {
    try {
      std::any value = evaluate(expression);
      std::cout << stringify(value) << "\n";
    } catch (RuntimeError error) {
      runtimeError(error);
    }
  }

private:
  std::any evaluate(std::shared_ptr<Expr> expr) {
   return expr->accept(*this);
  }

public:
  std::any visitBinaryExpr(std::shared_ptr<Binary> expr) override {
   std::any left = evaluate(expr->left);
   std::any right = evaluate(expr->right);
   switch(expr->op.type) {
    case TokenType::BANG_EQUAL: return !isEqual(left, right);
    case TokenType::EQUAL_EQUAL: return isEqual(left, right);
    case TokenType::GREATER:
      checkNumberOperands(expr->op, left, right);
      return std::any_cast<double>(left) > std::any_cast<double>(right);
    case TokenType::GREATER_EQUAL:
      checkNumberOperands(expr->op, left, right);
      return std::any_cast<double>(left) >= std::any_cast<double>(right);
   }

  }
};