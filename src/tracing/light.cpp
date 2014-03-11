#include "light.h"

namespace trace {

Light::Light(const Point& point, const RGB& _color) {
  p = point;
  c = _color;
}

Point Light::point() { return p; }
RGB Light::color() { return c; }

}
