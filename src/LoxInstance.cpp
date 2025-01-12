#include "LoxInstance.hpp"
#include "Token.hpp"
#include "LoxClass.hpp"
#include "Lox.hpp"
#include "LoxFunction.hpp"

std::any LoxInstance::get(Token& name) {
  auto elem = fields.find(name.lexeme);
  if(elem != fields.end()) {
    return elem->second;
  }
  std::shared_ptr<LoxFunction> method = klass->findMethod(name.lexeme);
  if(method != nullptr) return method->bind(shared_from_this());
  throw RuntimeError(name, "Undefined property '" + name.lexeme + "'.");
}

void LoxInstance::set(Token name, std::any value) {
  fields[name.lexeme] = std::move(value);
}

std::string LoxInstance::toString() {
  return klass->name + " instance";
}
