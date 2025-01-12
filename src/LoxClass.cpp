#include "LoxClass.hpp"
#include "Interpreter.hpp"
#include "LoxInstance.hpp"
#include "LoxFunction.hpp"

std::shared_ptr<LoxFunction> LoxClass::findMethod(std::string name) {
  auto elem = methods.find(name);
  if(elem != methods.end()) {
    return elem->second;
  }

  if(superClass != nullptr) {
    return superClass->findMethod(name);
  }

  return nullptr;
}

std::string LoxClass::toString() {
  return name;
}

std::any LoxClass::call(Interpreter& interpreter, std::vector<std::any>&& arguments) {
  auto instance = std::make_shared<LoxInstance>(shared_from_this());
  std::shared_ptr<LoxFunction> initializer = findMethod("init");
  if(initializer != nullptr) {
    initializer->bind(instance)->call(interpreter, std::move(arguments));
  }
  return instance;
}

int LoxClass::arity() {
  std::shared_ptr<LoxFunction> initializer = findMethod("init");
  if(initializer == nullptr)
    return 0;
  return initializer->arity();
}