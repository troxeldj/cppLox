#include "Parser.hpp"
#include "Expr.hpp"

Parser::Parser(std::vector<Token> tokens) : tokens(std::move(tokens)), current(0) {};

Expr* Parser::expression() {
}