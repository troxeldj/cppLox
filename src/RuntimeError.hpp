#ifndef __RUNTIMEERROR_H
#define __RUNTIMEERROR_H
#include <stdexcept>
#include "Token.hpp"

class RuntimeError : public std::runtime_error {
  public:
  const Token& token;
  RuntimeError(const Token& token, const std::string& message) 
  : std::runtime_error(message), token(token) {}

};
#endif