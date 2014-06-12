#include <cmath>
#include "lowlevel.h"

namespace trace {

Point::Point(double _x, double _y, double _z) {
  x = _x;
  y = _y;
  z = _z;
  undefined = false;
}

Point::Point() {
  undefined = true;
}

Point Point::sub(const Point& another) const {
  return Point(x - another.x, y - another.y, z - another.z);
}

bool Point::isDefined() { return !undefined; }

Vector::Vector(double _x, double _y, double _z) {
  x = _x;
  y = _y;
  z = _z;
  undefined = false;
}

Vector::Vector(const Point& a, const Point& b) {
  Point v = a.sub(b);
  x = v.x;
  y = v.y;
  z = v.z;
}

Vector::Vector() {
  undefined = true;
}

bool Vector::isDefined() { return !undefined; }


double Vector::mod() {
  return x * x + y * y + z * z;
}

Vector Vector::norm() {
  return Vector(x / ::sqrt(mod()), y / ::sqrt(mod()), z / ::sqrt(mod()));
}

RGB::RGB(double r, double g, double b) {
  red = r;
  green = g;
  blue = b;
}

RGB::RGB() {
  red = 0;
  green = 0;
  blue = 0;
}

RGB RGB::coef(double c) {
  return RGB(red * c, green * c, blue * c);
}

RGB RGB::mix(RGB another) {
  return RGB(another.red * red, another.green * green, another.blue * blue);
}

RGB RGB::add(RGB another) {
  return RGB(another.red + red, another.green + green, another.blue + blue);
}

RGB RGB::realmix(RGB another) {
  return RGB((another.red + red) / 2, (another.green + green) / 2, (another.blue + blue) / 2);
}
}
