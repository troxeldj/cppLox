#include <string>
#include <fstream>
#include <sstream>
#include "Lox.hpp"
#include "Scanner.hpp"
bool Lox::hadError = false;

std::string Lox::getLineText() {
  std::string line;
  std::getline(std::cin, line);
  return line;
}

// Function to get the contents of a file as a string
std::string Lox::fileToString(std::string& path) {
    std::ifstream t(path);
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}

void Lox::runFile(std::string& path) {
  std::string fileContents;
  try {
    fileContents = fileToString(path);
  } catch (const std::exception& e) {
    std::cerr << "Unable to read file at: " << path << std::endl;
    std::cerr << e.what() << std::endl;
  }
  run(fileContents);

  if(hadError) exit(65);
}

void Lox::runPrompt() {
  for(;;) {
    std::cout << "> ";
    std::string line = getLineText();
    if(line.empty()) break;
    run(line);
    hadError = false;
  }

}

void Lox::run(std::string& line) {
  Scanner scanner = Scanner(line);

}

void Lox::error(int line, const std::string& message) {
  report(line, "", message);
}

void Lox::report(int line, const std::string& where, const std::string& message) {
  std::cerr << "[line " << line << "] Error" << where << ": " << message << std::endl;
  hadError = true;
}