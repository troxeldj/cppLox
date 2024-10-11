#ifndef __UTIL_HPP__
#define __UTIL_HPP__
#include <iostream>
#include <any>

bool isInt(std::any& anyVal);
bool isFloat(std::any& anyVal);
bool isString(std::any& anyVal);
bool isDouble(std::any& anyVal);
int anyToInt(std::any& anyVal);
float anyToFloat(std::any&);
double anyToDouble(std::any&);
std::string anyToString(std::any&);
std::string convertAnyToString(std::any&);
#endif