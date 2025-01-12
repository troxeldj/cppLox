#ifndef __LOXINSTANCE_H
#define __LOXINSTANCE_H
#include <memory>
#include <any>
#include <map>
#include <string>

class LoxClass;
class Token;

class LoxInstance : public std::enable_shared_from_this<LoxInstance> {
  std::shared_ptr<LoxClass> klass;
  std::map<std::string, std::any> fields;
public:
  LoxInstance(std::shared_ptr<LoxClass> klass)
    : klass{std::move(klass)} {}
  std::any get(Token& name);
  void set(Token name, std::any value);
  std::string toString();
};
#endif