#ifndef __LOX_HPP
#define __LOX_HPP
#include <string>
#include "Scanner.hpp"
#include "RuntimeError.hpp"
#include "Interpreter.hpp"

class Lox {
  static Interpreter interpreter;
  static bool hadError; 
  static bool hadRuntimeError;
  Lox() = delete;

public:
  static std::string fileContentsToString(std::string filePath);
  static void run(std::string fileContents);
  static void runFile(std::string filePath);
  static void runPrompt();
  static void report(int line, std::string where, std::string message);
  static void error(int line, std::string message);
  static void error(Token& token, const std::string& message);
  static void error(const Token& token, const std::string& message);
  static void runtimeError(RuntimeError& error);
};

#endif