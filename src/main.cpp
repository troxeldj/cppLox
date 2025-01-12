#include <iostream>
#include "Lox.hpp"

int main(int argc, char* argv[]) {
  if (argc == 1) {
    Lox::runPrompt();   
  } else if (argc == 2) {
    std::string filePath = argv[1];
    Lox::runFile(filePath);
  } else {
    std::cerr << "Invalid number of arguments.\n";
    std::cerr << "Usage 'compiler <file_name>' or 'compiler'\n";
  }
  return 0;
}