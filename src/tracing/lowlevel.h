#pragma once

namespace trace {
struct Point {
  double x;
  double y;
  double z;

  bool undefined;

  Point(double _x, double _y, double _z);
  Point();
  Point sub(const Point& another) const;

  bool isDefined();
};

struct Vector {
  double x;
  double y;
  double z;
  bool undefined;

  Vector(double _x, double _y, double _z);
  Vector(const Point& a, const Point& b);
  Vector();

  bool isDefined();
  double mod();
  Vector norm();
};

struct RGB {
  double red;
  double green;
  double blue;

  RGB(double r, double g, double b);
  RGB();

  RGB coef(double c);
  RGB mix(RGB another);
  RGB add(RGB another);
};
}
