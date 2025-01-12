#include <iostream>
#include "Interpreter.hpp"
#include "RuntimeError.hpp"
#include "Lox.hpp"
#include "LoxFunction.hpp"
#include "LoxInstance.hpp"
#include "LoxReturn.hpp"
#include "LoxClass.hpp"


void Interpreter::resolve(std::shared_ptr<Expr> expr, int depth) {
  locals[expr] = depth;
}

std::any Interpreter::visitLiteralExpr(std::shared_ptr<Literal> expr) {
  return expr->value;
}

std::any Interpreter::visitGroupingExpr(std::shared_ptr<Grouping> expr) {
  return evaluate(expr->expression);
}

std::any Interpreter::visitCallExpr(std::shared_ptr<Call> expr) {
  std::any callee = evaluate(expr->callee);
  std::vector<std::any> arguments;
  for(std::shared_ptr<Expr> argument : expr->arguments) {
    arguments.push_back(evaluate(argument));
  }
  std::shared_ptr<LoxCallable> function; 
  if(callee.type() == typeid(std::shared_ptr<LoxFunction>)) {
    function = std::any_cast<std::shared_ptr<LoxFunction>>(callee);
  } else if(callee.type() == typeid(std::shared_ptr<LoxClass>)) {
    function = std::any_cast<std::shared_ptr<LoxClass>>(callee);
  } else {
    throw RuntimeError(expr->paren, "Can only call functions and classes.");
  }
  
  if(arguments.size() != function->arity()) {
    throw RuntimeError(expr->paren, "Expected " + std::to_string(function->arity()) + " arguments but got " + std::to_string(arguments.size()) + ".");
  }

  return function->call(*this, std::move(arguments));
}

std::any Interpreter::visitFunctionStmt(std::shared_ptr<Function> stmt) {
  auto function = std::make_shared<LoxFunction>(stmt, environment, false);
  environment->define(stmt->name.lexeme, function);
  return {};
}

std::any Interpreter::visitUnaryExpr(std::shared_ptr<Unary> expr) {
  std::any right = evaluate(expr->right);
  switch(expr->op.type) {
    case TokenType::BANG:
      return !isTruthy(right);
    case TokenType::MINUS:
      checkNumberOperands(expr->op, right, right);
      return -std::any_cast<double>(right);
  }
  return {};
}

std::any Interpreter::visitBinaryExpr(std::shared_ptr<Binary> expr) {
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
    case TokenType::LESS:
      checkNumberOperands(expr->op, left, right);
      return std::any_cast<double>(left) < std::any_cast<double>(right);
    case TokenType::LESS_EQUAL:
      checkNumberOperands(expr->op, left, right);
      return std::any_cast<double>(left) <= std::any_cast<double>(right);
    case TokenType::MINUS:
      checkNumberOperands(expr->op, left, right);
      return std::any_cast<double>(left) - std::any_cast<double>(right);
    case TokenType::PLUS:
      if(left.type() == typeid(double) && right.type() == typeid(double)) {
        return std::any_cast<double>(left) + std::any_cast<double>(right);
      }
      if(left.type() == typeid(std::string) && right.type() == typeid(std::string)) {
        return std::any_cast<std::string>(left) + std::any_cast<std::string>(right);
      }
      throw RuntimeError(expr->op, "Operands must be two numbers or two strings.");
    case TokenType::SLASH:
      checkNumberOperands(expr->op, left, right);
      return std::any_cast<double>(left) / std::any_cast<double>(right);
    case TokenType::STAR:
      checkNumberOperands(expr->op, left, right);
      return std::any_cast<double>(left) * std::any_cast<double>(right);
  }
  return {};
}

std::any Interpreter::visitAssignExpr(std::shared_ptr<Assign> expr) {
  std::any value = evaluate(expr->value);
  auto elem = locals.find(expr);
  if (elem != locals.end()) {
    int distance = elem->second;
    environment->assignAt(distance, expr->name, value);
  } else {
    globals->assign(expr->name, value);
  }

  return value;
}

std::any Interpreter::visitVariableExpr(std::shared_ptr<Variable> expr) {
  return lookUpVariable(expr->name, expr);
}

std::any Interpreter::lookUpVariable(Token name, std::shared_ptr<Expr> expr) {
  auto elem = locals.find(expr);
  if(elem != locals.end()) {
    int distance = elem->second;
    return environment->getAt(distance, name.lexeme);
  } else {
    return globals->get(name);
  }
}


std::any Interpreter::visitLogicalExpr(std::shared_ptr<Logical> expr) {
  std::any left = evaluate(expr->left);
  if(expr->op.type == TokenType::OR) {
    if(isTruthy(left)) return left;
  } else {
    if(!isTruthy(left)) return left;
  }
  return evaluate(expr->right);
}

std::any Interpreter::visitGetExpr(std::shared_ptr<Get> expr) {
  std::any object = evaluate(expr->object);
  if(object.type() == typeid(std::shared_ptr<LoxInstance>)) {
    return std::any_cast<std::shared_ptr<LoxInstance>>(object)->get(expr->name);
  }
  throw RuntimeError(expr->name, "Only instances have properties.");
}

std::any Interpreter::visitSetExpr(std::shared_ptr<Set> expr) {
  std::any object = evaluate(expr->object);
  if(object.type() != typeid(std::shared_ptr<LoxInstance>)) {
    throw RuntimeError(expr->name, "Only instances have fields.");
  }
  std::any value = evaluate(expr->value);
  std::any_cast<std::shared_ptr<LoxInstance>>(object)->set(expr->name, value);
  return value;
}

std::any Interpreter::visitThisExpr(std::shared_ptr<This> expr) {
  return lookUpVariable(expr->keyword, expr);
}

std::any Interpreter::visitSuperExpr(std::shared_ptr<Super> expr) {
  int distance = locals[expr];
  auto superclass = std::any_cast<std::shared_ptr<LoxClass>>(environment->getAt(distance, "super"));
  auto object = std::any_cast<std::shared_ptr<LoxInstance>>(environment->getAt(distance - 1, "this"));
  std::shared_ptr<LoxFunction> method = superclass->findMethod(expr->method.lexeme);
  if(method == nullptr) {
    throw RuntimeError(expr->method, "Undefined property '" + expr->method.lexeme + "'.");
  }
  return method->bind(object);
}
std::any Interpreter::visitIfStmt(std::shared_ptr<If> stmt) {
  std::any conditionAny = evaluate(stmt->condition);
  if(isTruthy(conditionAny)) {
    execute(stmt->thenBranch);
  } else if(stmt->elseBranch != nullptr) {
    execute(stmt->elseBranch);
  }
  return {};
}

std::any Interpreter::visitReturnStmt(std::shared_ptr<Return> stmt) {
  std::any value = nullptr;
  if(stmt->value != nullptr) value = evaluate(stmt->value);
  throw LoxReturn(value);
}
std::any Interpreter::visitWhileStmt(std::shared_ptr<While> stmt) {
  std::any condition = evaluate(stmt->condition);
  while(isTruthy(condition)) {
    execute(stmt->body);
    condition = evaluate(stmt->condition);
  }
  return {};
}

std::any Interpreter::visitClassStmt(std::shared_ptr<Class> stmt) {
  std::any superClass;
  if(stmt->superclass != nullptr) {
    superClass = evaluate(stmt->superclass);
    if(superClass.type() != typeid(std::shared_ptr<LoxClass>)) {
      throw RuntimeError(stmt->superclass->name, "Superclass must be a class.");
    }
  }
  environment->define(stmt->name.lexeme, nullptr);
  if(stmt->superclass != nullptr) {
    environment = std::make_shared<Environment>(environment);
    environment->define("super", superClass);
  }

  std::map<std::string, std::shared_ptr<LoxFunction>> methods;
  for(std::shared_ptr<Function> method : stmt->methods) {
    auto function = std::make_shared<LoxFunction>(method, environment, method->name.lexeme == "init");
    methods[method->name.lexeme] = function;
  }
  std::shared_ptr<LoxClass> superKlass = nullptr;
  if(superClass.type() == typeid(std::shared_ptr<LoxClass>)) {
    superKlass = std::any_cast<std::shared_ptr<LoxClass>>(superClass);
  }
  auto klass = std::make_shared<LoxClass>(stmt->name.lexeme, superKlass, methods);
  if(superKlass != nullptr) {
    environment = environment->enclosing;
  }

  environment->assign(stmt->name, std::move(klass));
  return {};
}

std::any Interpreter::evaluate(std::shared_ptr<Expr> expr) {
  return expr->accept(*this);
}

bool Interpreter::isTruthy(std::any& object) {
  if(object.type() == typeid(bool)) return std::any_cast<bool>(object);
  if(object.type() == typeid(bool)) return std::any_cast<bool>(object);
  return true;
}

void Interpreter::checkNumberOperands(const Token& op, std::any& left, std::any& right) {
  if(left.type() == typeid(double) && right.type() == typeid(double)) return;
  throw RuntimeError(op, "Operands must be numbers.");
}

bool Interpreter::isEqual(std::any& a, std::any& b) {
  if(a.type() == typeid(nullptr) && b.type() == typeid(nullptr)) return true;
  if(a.type() == typeid(nullptr)) return false;
  if(a.type() == typeid(std::string) && b.type() == typeid(std::string))
    return std::any_cast<std::string>(a) == std::any_cast<std::string>(b);
  if(a.type() == typeid(double) && b.type() == typeid(double))
    return std::any_cast<double>(a) == std::any_cast<double>(b);
  if(a.type() == typeid(bool) && b.type() == typeid(bool))
    return std::any_cast<bool>(a) == std::any_cast<bool>(b);
  return false;
}

std::string Interpreter::stringify(std::any& object) {
  if(object.type() == typeid(nullptr)) return "nil";
  if(object.type() == typeid(double)) {
    std::string text = std::to_string(std::any_cast<double>(object));
    if(text[text.length() - 2]  == '.' && text[text.length() - 1] == '0') {
      text = text.substr(0, text.length() - 2);
    }
    return text;
  }
  if(object.type() == typeid(std::string)) return std::any_cast<std::string>(object);
  if(object.type() == typeid(bool)) return std::any_cast<bool>(object) ? "true" : "false";
  if(object.type() == typeid(std::shared_ptr<LoxFunction>)) return std::any_cast<std::shared_ptr<LoxFunction>>(object)->toString();
  if(object.type() == typeid(std::shared_ptr<LoxClass>)) return std::any_cast<std::shared_ptr<LoxClass>>(object)->toString();
  if(object.type() == typeid(std::shared_ptr<LoxInstance>)) return std::any_cast<std::shared_ptr<LoxInstance>>(object)->toString();
  return "Error in stringify: Object type not recognized.\n";
}

void Interpreter::interpret(std::vector<std::shared_ptr<Stmt>>& statements) {
  try {
    for(auto& statement : statements) {
      execute(statement);
    }
  } catch (RuntimeError& error) {
    Lox::runtimeError(error);
  }
}

void Interpreter::execute(std::shared_ptr<Stmt> stmt) {
  stmt->accept(*this);
}

void Interpreter::executeBlock(const std::vector<std::shared_ptr<Stmt>>& statements, std::shared_ptr<Environment> environment) {
  std::shared_ptr<Environment> previous = this->environment;
  try {
    this->environment = environment;
    for (std::shared_ptr<Stmt> statement : statements) {
      execute(statement);
    }
  } catch (...) {
    this->environment = previous;
    throw;
  }
  this->environment = previous;
}

std::any Interpreter::visitBlockStmt(std::shared_ptr<Block> stmt) {
  executeBlock(stmt->statements, std::make_shared<Environment>(environment));
  return {};
}

std::any Interpreter::visitExpressionStmt(std::shared_ptr<Expression> stmt) {
  evaluate(stmt->expression);
  return {};
}

std::any Interpreter::visitPrintStmt(std::shared_ptr<Print> stmt) {
  std::any value = evaluate(stmt->expression);
  std::cout << stringify(value) << "\n";
  return {};
}

std::any Interpreter::visitVarStmt(std::shared_ptr<Var> stmt) {
  std::any value = nullptr;
  if(stmt->initializer != nullptr) {
    value = evaluate(stmt->initializer);
  }
  environment->define(stmt->name.lexeme, std::move(value));
  return {};
}