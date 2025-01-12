#ifndef __LOXCLASS_H
#define __LOXCLASS_H

#include <any>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "LoxCallable.hpp"

class Interpreter;
class LoxFunction;

class LoxClass : public LoxCallable, public std::enable_shared_from_this<LoxClass> {
  friend class LoxInstance;
  std::string name;
  std::shared_ptr<LoxClass> superClass;
  std::map <std::string, std::shared_ptr<LoxFunction>> methods;
public:
  LoxClass(std::string name, std::shared_ptr<LoxClass> superClass, std::map<std::string, std::shared_ptr<LoxFunction>> methods)
    : name{std::move(name)}, superClass{std::move(superClass)}, methods{std::move(methods)}
  {}
  std::shared_ptr<LoxFunction> findMethod(std::string name);
  std::string toString() override;
  std::any call(Interpreter& interpreter, std::vector<std::any>&& arguments) override;
  int arity() override;
};


#endif