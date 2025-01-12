#ifndef __LOXCALLABLE_H
#define __LOXCALLABLE_H

#include <any>
#include <vector>
#include <memory>
#include <string>
#include "Interpreter.hpp"

class LoxCallable {
public:
  virtual int arity() = 0;
  virtual std::any call(Interpreter& interpreter, std::vector<std::any>&& arguments) = 0;
  virtual std::string toString() = 0;
  virtual ~LoxCallable() = default;
};

#endif