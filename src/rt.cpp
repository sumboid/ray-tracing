#include <tuple>
#include <vector>
#include <string>

#include "frameworkstuff.h"
#include "bitmap.h"
#include "tracing/scene.h"
#include "tracing/camera.h"
#include "tracing/light.h"
#include "tracing/objects/sphere.h"
#include "tracing/lowlevel.h"

using std::tuple;
using std::tie;
using std::vector;
using std::string;

using trace::Camera;
using trace::Scene;
using trace::Sphere;
using trace::Light;
using trace::Point;
using trace::RGB;

using ts::system::System;
using ts::type::ID;

enum Size {
  RESOLUTION_X = 1000,
  RESOLUTION_Y = 1000
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

System* createSystem() {
  CellTools* ct = new CellTools;
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

int main()
{
  Scene* scene = createScene();
  System* system = createSystem();

  size_t nodesNumber = system->size();
  size_t id = system->id();

  size_t begin, end;
  tie(begin, end) = getInterval(nodesNumber, id, Size::RESOLUTION_Y);

  vector<Cell*> cells;
  for(size_t i = begin; i < end; ++i) {
    Camera* camera = new Camera(4, 4, 15, 5);
    camera->setViewPoint(Point(0, -20, 0));
    camera->setScene(scene);
    camera->setResolution(Size::RESOLUTION_X, Size::RESOLUTION_Y);
    camera->setPart(0, i, Size::RESOLUTION_X, i + 1);

    cells.push_back(new Cell(ID(id, i - begin, 0), camera));
  }
  for(auto cell : cells)
    system->addCell(cell);

  system->run();

  int realj = begin;
  bitmap_image bmp(Size::RESOLUTION_X, Size::RESOLUTION_Y);
  for(Cell* cell : cells) {
    RGB* table = cell->getResult();

    for(int i = 0; i < RESOLUTION_X; ++i) {
      for (int j = 0; j < 1; ++j) {
        RGB& color = table[j * RESOLUTION_X + i];
        bmp.set_pixel(i, realj, color.red * 255, color.green * 255, color.blue * 255);
      }
    }
    ++realj;
    delete[] table;

  }

  string first  = std::to_string(id);
  string ending = ".bmp";

  string filename = first + ending;
  bmp.save_image(filename);

  delete system;
  for(Cell* cell: cells)
    delete cell;
  return 0;
}
