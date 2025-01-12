#ifndef __INTERPRETER_H
#define __INTERPRETER_H
#include <chrono>
#include "Expr.hpp"
#include "Stmt.hpp"
#include "Environment.hpp"

class Interpreter : public ExprVisitor, public StmtVisitor {
  friend class LoxFunction;

public: 
  std::shared_ptr<Environment> globals{new Environment};
private: 
  std::shared_ptr<Environment> environment = globals; 
  std::map<std::shared_ptr<Expr>, int> locals;
public:
// Constructors
  Interpreter() {}
  ~Interpreter() = default;
  Interpreter(Interpreter& other) = delete;
  Interpreter(Interpreter&& other) = delete;
  Interpreter& operator=(Interpreter& other) = delete;

  // Expression overrides
  std::any visitLiteralExpr(std::shared_ptr<Literal> expr) override;
  std::any visitGroupingExpr(std::shared_ptr<Grouping> expr) override;
  std::any visitUnaryExpr(std::shared_ptr<Unary> expr) override;
  std::any visitBinaryExpr(std::shared_ptr<Binary> expr) override;
  std::any visitAssignExpr(std::shared_ptr<Assign> expr) override;
  std::any visitLogicalExpr(std::shared_ptr<Logical> expr) override;
  std::any visitVariableExpr(std::shared_ptr<Variable> expr) override;
  std::any visitGetExpr(std::shared_ptr<Get> expr) override;
  std::any visitSetExpr(std::shared_ptr<Set> expr) override;
  std::any visitThisExpr(std::shared_ptr<This> expr) override;
  std::any visitSuperExpr(std::shared_ptr<Super> expr) override;
  std::any visitCallExpr(std::shared_ptr<Call> expr) override;

  // Statement overrides
  std::any visitBlockStmt(std::shared_ptr<Block> stmt) override;
  std::any visitExpressionStmt(std::shared_ptr<Expression> stmt) override;
  std::any visitIfStmt(std::shared_ptr<If> stmt) override;
  std::any visitPrintStmt(std::shared_ptr<Print> stmt) override;
  std::any visitVarStmt(std::shared_ptr<Var> stmt) override;
  std::any visitWhileStmt(std::shared_ptr<While> stmt) override;
  std::any visitFunctionStmt(std::shared_ptr<Function> stmt) override;
  std::any visitReturnStmt(std::shared_ptr<Return> stmt) override;
  std::any visitClassStmt(std::shared_ptr<Class> stmt) override;
  void interpret(std::vector<std::shared_ptr<Stmt>>& statements);
  void executeBlock(const std::vector<std::shared_ptr<Stmt>>& statements, std::shared_ptr<Environment> environment);
  void resolve(std::shared_ptr<Expr> expr, int depth);
private:
  bool isTruthy(std::any& object);
  bool isEqual(std::any& a, std::any& b);
  std::string stringify(std::any& object);
  std::any evaluate(std::shared_ptr<Expr> expr);
  void execute(std::shared_ptr<Stmt> stmt);
  void checkNumberOperands(const Token& op, std::any& left, std::any& right);
  std::any lookUpVariable(Token name, std::shared_ptr<Expr> expr);
};

#endif