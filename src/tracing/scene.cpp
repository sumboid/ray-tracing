#include <algorithm>
#include "scene.h"

namespace trace {

Scene::Scene(int _iterations) { iterations = _iterations; }
Scene::~Scene() {}

void Scene::addObject(Object* o) { objects.push_back(o); }
void Scene::addLight(Light* l) { lights.push_back(l); }

Object* Scene::intersect(const Point& start, const Vector& ray) {
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

RGB Scene::illumination(const Point& start, const Vector& ray, int iteration) {
  Object* object = intersect(start, ray);
  if(object == 0) { // Not cool, but who cares
    return RGB(0, 0, 0);
  }

  Point point = object->interspect(start, ray);

  RGB color(0, 0, 0);

  for(Light* light : lights) {
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

    if(intersectionObject == 0) {
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

RGB Scene::getColor(const Point& start, const Vector& ray) {
  return illumination(start, ray, 0);
}
}
