#include <iostream>
#include "Expr.hpp"
#include "Binary.hpp"
#include "Unary.hpp"
#include "Literal.hpp"
#include "Grouping.hpp"
#include "Lox.hpp"
#include "AstPrinter.hpp"
int main(int argc, char* argv[]) {
    // if(argc > 2) {
      // std::cerr << "Usage: lox [script]" << std::endl;
      // return 64;
    // } else if(argc == 2) {
      // std::string fileName = argv[1];
      // Lox::runFile(fileName);
    // } else {
      // Lox::runPrompt();
    // }
    // return 0;  
    Literal lit = Literal(123);
    Literal lit2 = Literal(45.67);
    Unary unary = Unary(Token(TokenType::MINUS, "-", std::any{}, 1), &lit);
    Grouping group = Grouping(&lit2);
    Binary bin = Binary(&unary, Token(TokenType::STAR, "*", std::any{}, 1), &group);
    Expr* expression = &bin;

    AstPrinter().print(expression);
}