#ifndef __LOXRETURN_H
#define __LOXRETURN_H

#include <any>

struct LoxReturn {
  std::any value;
  LoxReturn(std::any value) : value{value} {}
};

#endif