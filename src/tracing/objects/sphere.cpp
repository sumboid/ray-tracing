#include "sphere.h"
#include <cmath>
#include <limits>
namespace trace {

Sphere::Sphere(const Point& point, double _r, const RGB& _color) {
  p = point;
  r = _r;
  c = _color;
}

Point Sphere::interspect(const Point& start, const Vector& ray) {
  Vector v(start, p);

  double d = (v.x * ray.x + v.y * ray.y + v.z * ray.z) *
             (v.x * ray.x + v.y * ray.y + v.z * ray.z) -
             ((v.x * v.x + v.y * v.y + v.z * v.z) - r * r);
  if(d < 0) {
    return Point();
  }

  double t1 = -(v.x * ray.x + v.y * ray.y + v.z * ray.z) + ::sqrt(d);
  double t2 = -(v.x * ray.x + v.y * ray.y + v.z * ray.z) - ::sqrt(d);

  double t = 0;
  double epsilon = 0.00001;

  if(t1 < epsilon)
  {
    if(t2 < epsilon)
      return Point();
    else
      t = t2;
  }
  else
    if(t2 < epsilon)
      t = t1;
    else
      t = (t1 > t2) ? t2 : t1;

  return Point(ray.x * t + start.x,
               ray.y * t + start.y,
               ray.z * t + start.z);

}

Point Sphere::point() { return p; }
RGB Sphere::color() { return c; }

}
