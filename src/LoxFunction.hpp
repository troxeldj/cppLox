#ifndef __LOXFUNCTION_H
#define __LOXFUNCTION_H

#include <any>
#include <vector>
#include <memory>
#include <string>
#include "LoxCallable.hpp"

struct Environment;
struct Function;
struct LoxInstance;



class LoxFunction : public LoxCallable{
  std::shared_ptr<Function> declaration;
  std::shared_ptr<Environment> closure;
  bool isInitializer;
public:
  LoxFunction(std::shared_ptr<Function> declaration, std::shared_ptr<Environment> closure, bool isInitializer)
    : declaration{std::move(declaration)}, closure{std::move(closure)}, isInitializer{isInitializer}
  {}
  std::string toString() override;
  int arity() override;
  std::shared_ptr<LoxFunction> bind(std::shared_ptr<LoxInstance> instance);
  std::any call(Interpreter& interpreter, std::vector<std::any>&& arguments) override;
};


#endif