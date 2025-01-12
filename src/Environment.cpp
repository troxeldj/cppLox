#include "Environment.hpp"
#include "RuntimeError.hpp"

void Environment::define(const std::string& name, std::any value) {
  values[name] = std::move(value);
}

std::any Environment::get(const Token& name) {
  auto elem = values.find(name.lexeme);
  if(elem != values.end()) 
    return elem->second;
  if(enclosing != nullptr) return enclosing->get(name);
  throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}

void Environment::assign(const Token& name, std::any value) {
  auto elem = values.find(name.lexeme);
  if(elem != values.end()) {
    elem->second = value;
    return;
  }
  if(enclosing != nullptr) {
    enclosing->assign(name, value);
    return;
  }
  throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
}

std::shared_ptr<Environment> Environment::ancestor(int distance) {
  std::shared_ptr<Environment> environment = shared_from_this();
  for(int i = 0; i < distance; i++) {
    environment = environment->enclosing;
  }
  return environment;
}

std::any Environment::getAt(int distance, const std::string& name) {
  return ancestor(distance)->values[name];
}

void Environment::assignAt(int distance, const Token& name, std::any& value) {
  ancestor(distance)->values[name.lexeme] = value;
}