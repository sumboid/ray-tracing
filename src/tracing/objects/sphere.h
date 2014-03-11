#pragma once
#include "object.h"
#include "../lowlevel.h"

namespace trace {

class Sphere : public Object {
private:
  Point p;
  double r;
  RGB c;

public:
  Sphere(const Point& point, double _r, const RGB& _color);
  virtual Point interspect(const Point& start, const Vector& ray);
  virtual Point point();
  virtual RGB color();
};

}
