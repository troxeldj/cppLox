#ifndef __LOX_HPP__
#define __LOX_HPP__
#include <iostream>

class Lox {
  static bool hadError;
  static void report(int, const std::string&, const std::string&);
public:
    static void error(int, const std::string&);
    static std::string getLineText();
    static void runFile(std::string&);
    static void runPrompt();
    static void run(std::string&);
    static std::string fileToString(std::string&);
};

#endif