#pragma once
#include <vector>
#include "lowlevel.h"
#include "light.h"
#include "objects/object.h"

namespace trace {

class Scene {
private:
  std::vector<Object*> objects;
  std::vector<Light*> lights;

  int iterations;

public:
  Scene(int _iterations);
  ~Scene();

  void addObject(Object* o);
  void addLight(Light* l);

  Object* intersect(const Point& start, const Vector& ray);
  RGB illumination(const Point& start, const Vector& ray, int iteration);
  RGB getColor(const Point& start, const Vector& ray);
};

}
