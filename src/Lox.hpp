#pragma once
#include <iostream>

std::string fileContentsToString(std::string);

struct Token;

struct Lox {
  static bool hadError;
  static void runFile(std::string);
  static void run(std::string);
  static void runPrompt();
  static void error(int line, std::string message);
  static void error(const Token& , std::string);
  static void report(int line, std::string where, std::string message);
};
