#include "util.hpp"

bool isInt(std::any& anyVal) {
    return anyVal.type() == typeid(int);
}

bool isFloat(std::any& anyVal) {
    return anyVal.type() == typeid(float);
}

bool isString(std::any& anyVal) {
    return anyVal.type() == typeid(std::string);
}

bool isDouble(std::any& anyVal) {
    return anyVal.type() == typeid(double);
}

int anyToInt(std::any& anyVal) {
    return std::any_cast<int>(anyVal);
}

float anyToFloat(std::any& anyVal) {
    return std::any_cast<float>(anyVal);
}

double anyToDouble(std::any& anyVal) {
    return std::any_cast<double>(anyVal);
}

std::string anyToString(std::any& anyVal) {
    return std::any_cast<std::string>(anyVal);
}

std::string convertAnyToString(std::any& anyVal) {
    if (isInt(anyVal)) {
        return std::to_string(anyToInt(anyVal));
    } else if (isFloat(anyVal)) {
        return std::to_string(anyToFloat(anyVal));
    } else if (isString(anyVal)) {
        return anyToString(anyVal);
    } else if (isDouble(anyVal)) {
        return std::to_string(anyToDouble(anyVal));
    } else {
        throw std::runtime_error("Unsupported type in std::any");
    }
}