#include <fstream>
#include <sstream>
#include <filesystem>
#include <exception>
#include "Lox.hpp"
#include "AstPrinter.hpp"
#include "Parser.hpp"
#include "Interpreter.hpp"
#include "Resolver.hpp"

bool Lox::hadError = false;
bool Lox::hadRuntimeError = false;
Interpreter Lox::interpreter = Interpreter();

std::string Lox::fileContentsToString(std::string filePath) {
  std::ifstream t(filePath);
  std::stringstream buffer;
  buffer << t.rdbuf();
  return buffer.str();
}

void Lox::run(std::string contents) {
  Scanner scanner = Scanner(contents);
  std::vector<Token>& tokens = scanner.scanTokens();

  Parser parser = Parser(tokens);
  std::vector<std::shared_ptr<Stmt>> statements = parser.parse();
  if(Lox::hadError) return;

  Resolver resolver = Resolver(interpreter);
  resolver.resolve(statements);

  if(Lox::hadError) return;

  interpreter.interpret(statements);
}

void Lox::runFile(std::string filePath) {
  // Check if file exists
  if(!std::filesystem::exists(filePath)) 
    throw std::runtime_error("File does not exist at " + filePath);
  // Get file contents 
  std::string fileContents = Lox::fileContentsToString(filePath);
  // run
  Lox::run(fileContents);
  if(Lox::hadError) exit(65);
  if(Lox::hadRuntimeError) exit(70);
}

void Lox::runPrompt() {
  std::string line;
  while(true) {
    std::cout << "> ";
    std::getline(std::cin, line);
    if(line == "" || (line == "exit" || line == "quit")) {
      break;
    }
    Lox::run(line);
    hadError = false;
  }
}

void Lox::report(int line, std::string where, std::string message) {
  std::cerr << "[line " << line << "] Error" << where << ": " << message << "\n";
  Lox::hadError = true;
}

void Lox::error(int line, std::string message) {
  Lox::report(line, "", message);
}

void Lox::error(const Token& token, const std::string& message) {
  if(token.type == TokenType::END_OF_FILE) {
    Lox::report(token.line, " at end", message);
  } else {
    Lox::report(token.line, " at '" + token.lexeme + "'", message);
  }
}

void Lox::error(Token& token, const std::string& message) {
  if(token.type == TokenType::END_OF_FILE) {
    Lox::report(token.line, " at end", message);
  } else {
    Lox::report(token.line, " at '" + token.lexeme + "'", message);
  }
}

void Lox::runtimeError(RuntimeError& error) {
  std::cerr << error.what() << "\n[line " << error.token.line << "]\n";
  hadRuntimeError = true;
}