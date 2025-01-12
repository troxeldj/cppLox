#ifndef __ENVIRONMENT_HPP
#define __ENVIRONMENT_HPP
#include <iostream>
#include <map>
#include <any>
#include <memory>
#include "Token.hpp"

class Environment : public std::enable_shared_from_this<Environment> {
  friend class Interpreter;

  std::shared_ptr<Environment> enclosing;
  std::map<std::string, std::any> values;
public:
// Constructors
  Environment() 
    : values{}, enclosing{nullptr} {}
  Environment(std::shared_ptr<Environment> enclosing)
    : values{}, enclosing{std::move(enclosing)} {}
  ~Environment() = default;
  Environment(Environment& other) = delete;
  Environment(Environment&& other) = delete;
  Environment& operator=(Environment& other) = delete;

// Methods
  void define(const std::string& name, std::any value);
  std::any get(const Token& name);
  void assign(const Token& name, std::any value);
  std::any getAt(int distance, const std::string& name);
  std::shared_ptr<Environment> ancestor(int distance);
  void assignAt(int distance, const Token& name, std::any& value);
};

#endif