#pragma once
#include "Expr.hpp"
#include <string>
#include <algorithm>
#include <cassert>
#include <sstream>
#include <memory>
#include <vector>

inline std::vector<std::string> split(std::string& s, const std::string& delimiter) {
  std::vector<std::string> tokens;
  size_t pos = 0;
  std::string token;
  while ((pos = s.find(delimiter)) != std::string::npos) {
      token = s.substr(0, pos);
      tokens.push_back(token);
      s.erase(0, pos + delimiter.length());
  }
  tokens.push_back(s);

  return tokens;
}
  // trim from start (in place)
inline void ltrim(std::string &s) {
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
      return !std::isspace(ch);
  }));
}

// trim from end (in place)
inline void rtrim(std::string &s) {
  s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
      return !std::isspace(ch);
  }).base(), s.end());
}

inline void trim(std::string &s) {
  ltrim(s);
  rtrim(s);
}

inline std::string toLowerCase(std::string s) {
  std::string out;

  for(char& c : s)  {
    out.push_back(std::tolower(c));
  }
  return out;
}


class ASTPrinter : public ExprVisitor {
public:
  std::string print(std::shared_ptr<Expr> expr) {
    return std::any_cast<std::string>(expr->accept(*this));
  }

  std::any visitBinaryExpr(std::shared_ptr<Binary> expr) override {
    return parenthesize(expr->op.lexeme, expr->left, expr->right);
  }

  std::any visitGroupingExpr(std::shared_ptr<Grouping> expr) override {
    return parenthesize("group", expr->expression);
  }

  std::any visitLiteralExpr(std::shared_ptr<Literal> expr) override {
    auto& value_type = expr->value.type();

    if(value_type == typeid(nullptr)) {
        return "nil";
    } else if(value_type == typeid(std::string)) {
      return std::any_cast<std::string>(expr->value);
    } else if (value_type == typeid(double)) {
      return std::to_string(std::any_cast<double>(expr->value));
    } else if (value_type == typeid(bool)) {
      return std::any_cast<bool>(expr->value)? "true" : "false";
    }

    return "Error in visitLiteralExpr : Literal Type is not recognized.";
  }

  std::any visitUnaryExpr(std::shared_ptr<Unary> expr) override {
    return parenthesize(expr->op.lexeme, expr->right);
  }

private:
  template <class... E>
  std::string parenthesize(std::string name, E... expr) {
   assert((... && std::is_same_v<E, std::shared_ptr<Expr>>));

   std::ostringstream builder;

   builder << "(" << name;
   ((builder << " " << print(expr)), ...);
   builder << ")";

   return builder.str();
  }
};