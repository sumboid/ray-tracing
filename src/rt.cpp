#include <tuple>
#include <vector>
#include <string>
#include <set>

#include "frameworkstuff.h"
#include "bitmap.h"
#include "tracing/scene.h"
#include "tracing/camera.h"
#include "tracing/light.h"
#include "tracing/objects/sphere.h"
#include "tracing/lowlevel.h"

#include <ts/system/System.h>

#define FRAGMENTS_NUMBER 25

using std::tuple;
using std::tie;
using std::vector;
using std::string;
using std::set;

using trace::Camera;
using trace::Scene;
using trace::Sphere;
using trace::Light;
using trace::Point;
using trace::RGB;

using ts::system::System;
using ts::type::ID;

enum Size {
  RESOLUTION_X = 5000,
  RESOLUTION_Y = 5000
};

Scene* createScene() {
  Scene* scene = new Scene(100);

  scene->addObject(new Sphere(Point(0, 7, 2), 1, RGB(1, 0.3, 0.3)));
  scene->addObject(new Sphere(Point(-3, 11, -2), 2, RGB(0.3, 0.3, 1)));
  scene->addObject(new Sphere(Point(0, 8, -2), 1, RGB(0.3, 1, 0.3)));
  scene->addObject(new Sphere(Point(1.5, 7, 0.5), 1, RGB(0.5, 0.5, 0.5)));
  scene->addObject(new Sphere(Point(-2, 6, 1), 0.7, RGB(0.3, 1, 1)));
  scene->addObject(new Sphere(Point(2.2, 8, 0), 1, RGB(0.5, 0.5, 0.5)));
  scene->addObject(new Sphere(Point(4, 10, 1), 0.7, RGB(0.3, 0.3, 1)));

  scene->addLight(new Light(Point(-15, -15, 0), RGB(0.5, 0.5, 0.5)));
  scene->addLight(new Light(Point(1, 0, 1), RGB(0.5, 0.5, 0.5)));
  scene->addLight(new Light(Point(0, 6, -10), RGB(0.5, 0.5, 0.5)));

  return scene;
}

System* createSystem(Scene* scene, Camera* camera) {
  FragmentTools* ct = new FragmentTools(scene, camera);
  ReduceDataTools* rt = new ReduceDataTools;
  return new System(ct, rt);
}

tuple<size_t, size_t> getInterval(size_t size, size_t id, size_t partsNumber) {
  size_t end = partsNumber % size;
  size_t partSize = partsNumber / size;

  if(id >= size - end) {
    size_t some = id - (size - end);
    return tuple<size_t, size_t>(partSize * id + some, partSize * id + some + partSize + 1);
  }
  else {
    return tuple<size_t, size_t>(partSize * id, partSize * id + partSize);
  }
}

set<tuple<size_t, size_t>> getInterval(size_t size, size_t id, size_t linesNumber, size_t fragmentsNumber) {
  set<tuple<size_t, size_t>> result;
  size_t begin, end;
  tie(begin, end) = getInterval(size, id, linesNumber);

  for(size_t i = 0; i < fragmentsNumber; ++i) {
    size_t lb, le;
    tie(lb, le) = getInterval(fragmentsNumber, i, end - begin);
    result.emplace(lb + begin, le + begin);
  }

  return result;
}

Camera* createCamera(Scene* scene) {
  Camera* camera = new Camera(4, 4, 15, 5);
  camera->setViewPoint(Point(0, -60, 0));
  camera->setScene(scene);
  camera->setResolution(Size::RESOLUTION_X, Size::RESOLUTION_Y);
  return camera;
}

std::map<int, double> balancer(uint64_t, std::map<int, uint64_t>) {
  return std::map<int, double>();
}

int main()
{
  Scene* scene = createScene();
  System* system = createSystem(scene, createCamera(scene));
  system->setBalancer(balancer);

  size_t nodesNumber = system->size();
  size_t id = system->id();

  auto split = getInterval(nodesNumber, id, Size::RESOLUTION_Y, FRAGMENTS_NUMBER);

  if(id == 0) {
    Fragment* endFragment = new Fragment(ID(-1, -1, -1), 0);
    for(size_t i = 0; i < nodesNumber; ++i) {
      auto split = getInterval(nodesNumber, i, Size::RESOLUTION_Y, FRAGMENTS_NUMBER);
      for(size_t j = 0; j < split.size(); ++j) {
        endFragment->addNeighbour(ID(i, j, 0), i);
      }
    }
    system->addFragment(endFragment);
  }

  vector<Fragment*> fs;
  size_t count = 0;
  for(auto& i: split) {
    Camera* camera = createCamera(scene);
    size_t b, e;
    tie(b, e) = i;
    camera->setPart(0, b, Size::RESOLUTION_X, e);
    fs.push_back(new Fragment(ID(id, count++, 0), camera));
  }
  for(auto f : fs) {
    f->addNeighbour(ID(-1, -1, -1), 0);
    system->addFragment(f);
  }
  system->run();

  delete system;
  return 0;
}
