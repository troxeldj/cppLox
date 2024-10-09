#ifndef __LOX_HPP__
#define __LOX_HPP__
#include <iostream>

class Lox {
  static bool hadError;
  static void report(int line, const std::string& where, const std::string& message);
public:
    static void error(int line, const std::string& message);
    static std::string getLineText();
    static void runFile(std::string& path);
    static void runPrompt();
    static void run(std::string& line);
    static std::string fileToString(std::string& path);
};

#endif