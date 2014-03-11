#include <iostream>
#include <limits>
#include <vector>
#include <cmath>
#include <algorithm>
#include "bitmap.h"

struct Point {
  double x;
  double y;
  double z;

  bool undefined;

  Point(double _x, double _y, double _z) {
    x = _x;
    y = _y;
    z = _z;
    undefined = false;
  }

  Point() {
    undefined = true;
  }

  Point sub(const Point& another) const {
    return Point(x - another.x, y - another.y, z - another.z);
  }

  bool isDefined() { return !undefined; }
};

struct Vector {
  double x;
  double y;
  double z;
  bool undefined;

  Vector(double _x, double _y, double _z) {
    x = _x;
    y = _y;
    z = _z;
    undefined = false;
  }

  Vector(const Point& a, const Point& b) {
    Point v = a.sub(b);
    x = v.x;
    y = v.y;
    z = v.z;
  }

  Vector() {
    undefined = true;
  }

  bool isDefined() { return !undefined; }


  double mod() {
    return x * x + y * y + z * z;
  }

  Vector norm() {
    return Vector(x / ::sqrt(mod()), y / ::sqrt(mod()), z / ::sqrt(mod()));
  }
};

struct RGB {
  double red;
  double green;
  double blue;

  RGB(double r, double g, double b) {
    red = r;
    green = g;
    blue = b;
  }

  RGB() {
    red = 0;
    green = 0;
    blue = 0;
  }

  RGB coef(double c) {
    return RGB(red * c, green * c, blue * c);
  }

  RGB mix(RGB another) {
    return RGB(another.red * red, another.green * green, another.blue * blue);
  }

  RGB add(RGB another) {
    return RGB(another.red + red, another.green + green, another.blue + blue);
  }
};

class Object {
public:
  virtual ~Object() {}
  virtual Point interspect(const Point& start, const Vector& ray) = 0;
  virtual Point point() = 0;
  virtual RGB color() = 0;
};

class Sphere : public Object {
private:
  Point p;
  double r;
  RGB c;

public:
  Sphere(const Point& point, double _r, const RGB& _color) {
    p = point;
    r = _r;
    c = _color;
  }

  virtual Point interspect(const Point& start, const Vector& ray) {
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
    double epsilon = 0.00000001;

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

  virtual Point point() { return p; }
  virtual RGB color() { return c; }
};

struct Light {
  Point p;
  RGB c;

  Light(const Point& point, const RGB& _color) {
    p = point;
    c = _color;
  }

  Point point() { return p; }
  RGB color() { return c; }
};

class Scene {
private:
  std::vector<Object*> objects;
  std::vector<Light*> lights;

  int iterations;

public:
  Scene(int _iterations) { iterations = _iterations; }
  ~Scene() {}

  void addObject(Object* o) { objects.push_back(o); }
  void addLight(Light* l) { lights.push_back(l); }

  Object* intersect(const Point& start, const Vector& ray) {
    Object* min  = *std::min_element(objects.begin(), objects.end(),
                                     [=](Object* x, Object* y) {
      Point interspectX = x->interspect(start, ray);
      Point interspectY = y->interspect(start, ray);
      if(!interspectX.isDefined()) return false;
      if(!interspectY.isDefined()) return true;

      return Vector(interspectX, start).mod() < Vector(interspectY, start).mod();
    });

    if(min->interspect(start, ray).isDefined()) {
      return min;
    }
    else {
      return 0;
    }
  }

  RGB illumination(const Point& start, const Vector& ray, int iteration)
  {
    Object* object = intersect(start, ray);
    if(object == 0) { // Not cool, but who cares
      return RGB(0, 0, 0);
    }

    Point point = object->interspect(start, ray);

    RGB color(0, 0, 0);

    for(Light* light : lights)
    {
      Vector lightRay(light->point(), point);
      lightRay = lightRay.norm();
      double lightDistance = lightRay.mod();

      Object* intersectionObject = intersect(point, lightRay);

      if(intersectionObject != 0) {
        Point intersectionPoint = intersectionObject->interspect(point, lightRay).sub(point);
        double obstacleDistance = Vector(intersectionPoint, Point(0, 0, 0)).mod();

        if(obstacleDistance > lightDistance) {
          intersectionObject = 0;
        }
      }

      if(intersectionObject == 0)  {
        Vector normal = Vector(point, object->point()).norm();

        double cosine = normal.x * (-ray.x) + normal.y * (-ray.y) + normal.z * (-ray.z);
        Vector reflectedRay(ray.x + 2 * cosine * normal.x,
                             ray.y + 2 * cosine * normal.y,
                             ray.z + 2 * cosine * normal.z);

        cosine = reflectedRay.x * lightRay.x + reflectedRay.y * lightRay.y + reflectedRay.z * lightRay.z;
        if(cosine < 0)
          cosine = 0;
        //cosine = reflectedRay.mod();
        color = color.add(light->color().mix(object->color().coef(cosine)));
      }
    }
    color = color.add(RGB(0.1, 0.1, 0.1).mix(object->color()));

    if(iteration != iterations)
    {
      Vector normal = Vector(point, object->point()).norm();

      double cosine = normal.x * (-ray.x) + normal.y * (-ray.y) + normal.z * (-ray.z);
      Vector reflectedRay(ray.x + 2 * cosine * normal.x,
                          ray.y + 2 * cosine * normal.y,
                          ray.z + 2 * cosine * normal.z);

      RGB reflection = illumination(point, reflectedRay.norm(), iteration + 1);
      color = color.add(reflection.mix(object->color()));
    }

    return color;
  }

  RGB getColor(const Point& start, const Vector& ray) {
    return illumination(start, ray, 0);
  }
};

int main()
{

  double backgroundSizeX = 4;
  double backgroundSizeZ = 4;
  double backgroundDistance = 15;

  double imagePlaneDistance = 5;
  double imagePlaneSizeX;
  double imagePlaneSizeZ;

  int imagePlaneResolutionX = 1000;
  int imagePlaneResolutionZ = 1000;
  bitmap_image bmp(imagePlaneResolutionX, imagePlaneResolutionZ);

  Scene scene(100);
  scene.addObject(new Sphere(Point(0, 7, 2), 1, RGB(1, 0.3, 0.3)));
  scene.addObject(new Sphere(Point(-3, 11, -2), 2, RGB(0.3, 0.3, 1)));
  scene.addObject(new Sphere(Point(0, 8, -2), 1, RGB(0.3, 1, 0.3)));
  scene.addObject(new Sphere(Point(1.5, 7, 0.5), 1, RGB(0.5, 0.5, 0.5)));
  scene.addObject(new Sphere(Point(-2, 6, 1), 0.7, RGB(0.3, 1, 1)));
  scene.addObject(new Sphere(Point(2.2, 8, 0), 1, RGB(0.5, 0.5, 0.5)));
  scene.addObject(new Sphere(Point(4, 10, 1), 0.7, RGB(0.3, 0.3, 1)));

  scene.addLight(new Light(Point(-15, -15, 0), RGB(0.5, 0.5, 0.5)));
  scene.addLight(new Light(Point(1, 0, 1), RGB(0.5, 0.5, 0.5)));
  scene.addLight(new Light(Point(0, 6, -10), RGB(0.5, 0.5, 0.5)));

  Point viewPoint(0, -20, 0);

  imagePlaneSizeX = backgroundSizeX * imagePlaneDistance / backgroundDistance;
  imagePlaneSizeZ = backgroundSizeZ * imagePlaneDistance / backgroundDistance;

  for(int ix = 0; ix < imagePlaneResolutionX; ix++)
    for(int iz = 0; iz < imagePlaneResolutionZ; iz++)
    {
      Vector ray (ix*imagePlaneSizeX/imagePlaneResolutionX-imagePlaneSizeX/2,
                  imagePlaneDistance,
                  iz*imagePlaneSizeZ/imagePlaneResolutionZ-imagePlaneSizeZ/2);

      ray = ray.norm();

      RGB color = scene.getColor(viewPoint, ray);

      if(color.red > 1)
        color.red = 1;
      if(color.blue > 1)
        color.blue = 1;
      if(color.green > 1)
        color.green = 1;

      bmp.set_pixel(ix, iz, 255*color.red, 255*color.green, 255*color.blue);
    }

  char filename[] = "raytracing00.bmp";
  bmp.save_image(filename);
  return 0;
}
