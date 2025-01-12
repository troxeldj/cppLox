#include "LoxFunction.hpp"
#include "Stmt.hpp"
#include "LoxReturn.hpp"
#include "LoxInstance.hpp"

std::string LoxFunction::toString() {
  return "<fn " + declaration->name.lexeme + ">"; 
}

int LoxFunction::arity() {
  return declaration->params.size();
}

std::any LoxFunction::call(Interpreter& interpreter, std::vector<std::any>&& arguments) {
  std::shared_ptr<Environment> environment = std::make_shared<Environment>(closure);
  for(int i = 0; i < declaration->params.size(); i++) {
    environment->define(declaration->params.at(i).lexeme, arguments.at(i));
  }

  try {
    interpreter.executeBlock(declaration->body, environment);
  } catch (LoxReturn& returnValue) {
    if(isInitializer) return closure->getAt(0, "this");
    return returnValue.value;
  }
  if(isInitializer) return closure->getAt(0, "this");
  return nullptr;
}

std::shared_ptr<LoxFunction> LoxFunction::bind(std::shared_ptr<LoxInstance> instance) {
  auto environment = std::make_shared<Environment>(closure);
  environment->define("this", instance);
  return std::make_shared<LoxFunction>(declaration, environment, isInitializer);
}