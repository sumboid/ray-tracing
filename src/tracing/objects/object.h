#pragma once
#include "../lowlevel.h"

namespace trace {

class Object {
public:
  virtual ~Object() {}
  virtual Point interspect(const Point& start, const Vector& ray) = 0;
  virtual Point point() = 0;
  virtual RGB color() = 0;
};

}
