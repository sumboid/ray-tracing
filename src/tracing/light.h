#pragma once
#include "lowlevel.h"

namespace trace {

struct Light {
  Point p;
  RGB c;

  Light(const Point& point, const RGB& _color);
  Point point();
  RGB color();
};

}
