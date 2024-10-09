#include "Lox.hpp"

int main(int argc, char* argv[]) {
    if(argc > 2) {
      std::cerr << "Usage: lox [script]" << std::endl;
      return 64;
    } else if(argc == 2) {
      std::string fileName = argv[1];
      Lox::runFile(fileName);
    } else {
      Lox::runPrompt();
    }
    return 0;  
}