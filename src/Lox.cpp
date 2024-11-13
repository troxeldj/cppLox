#include "Lox.hpp"
#include "Util.hpp"
#include <fstream>
#include <sstream>
#include "Scanner.hpp"
#include "Parser.hpp"
#include "RuntimeError.hpp"
#include "Interpreter.hpp"

bool Lox::hadError = false;
bool Lox::hadRuntimeError = false;

Interpreter interpreter = Interpreter();

std::string fileContentsToString(std::string path) {
  std::ifstream t(path);
  std::stringstream buffer;
  buffer << t.rdbuf();
  return buffer.str();
}

void Lox::runFile(std::string path) {
  std::string fileContents = fileContentsToString(path);
  Lox::run(fileContents);
  if(hadError) std::exit(65);
  if(hadRuntimeError) std::exit(70);
}

void Lox::run(std::string source) {
  Scanner scanner = Scanner(source);
  std::vector<Token> tokens = scanner.scanTokens();
  Parser parser{tokens};
  std::vector<std::shared_ptr<Stmt>> statements = parser.parse();

  if(hadError) return;

  interpreter.interpret(statements); 
}

void Lox::runPrompt() {
  std::string line;
  while(true) {
    std::cout << "> ";
    std::cin >> line;
    if(line == Util::EXIT_KEYWORD)
      break;
    Lox::run(line);
    Lox::hadError = false;
    Lox::hadRuntimeError = false;
  }
  exit(0);
}
void Lox::error(int line, std::string message) {
  report(line, "", message);
}

void Lox::error(const Token& token, std::string message) {
  if(token.type == TokenType::END_OF_FILE) {
    report(token.line, " at end", message);
  } else {
    report(token.line, "at '" + token.lexeme + "'", message);
  }
}

void Lox::report(int line, std::string where, std::string message) {
  std::cerr << "[line " << line << "] Error " << where << ": " << message << "\n";
  Lox::hadError = true;
}

void Lox::runtimeError(const RuntimeError& error) {
  std::cerr << error.what() << "\n[line " << error.token.line << "]\n";
  Lox::hadRuntimeError = true;
}

int main(int argc, char* argv[]) {
  if(argc == 1) Lox::runPrompt();
  else if(argc == 2) Lox::runFile(argv[1]);
  else {
    std::cout << "Usage: lox <file_name>\n";
    exit(64);
  }
  return 0;
}