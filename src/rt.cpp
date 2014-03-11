#include "bitmap.h"
#include "tracing/scene.h"
#include "tracing/camera.h"
#include "tracing/light.h"
#include "tracing/objects/sphere.h"
#include "tracing/lowlevel.h"

using trace::Camera;
using trace::Scene;
using trace::Sphere;
using trace::Light;
using trace::Point;
using trace::RGB;

int main()
{
  int x = 3000;
  int y = 3000;
  bitmap_image bmp(x, y);

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

  Camera camera(4, 4, 15, 5);
  camera.setViewPoint(Point(0, -20, 0));
  camera.setScene(&scene);
  camera.setResolution(x, y);
  auto table = camera.run();

  for(int i = 0; i < x; ++i) {
    for (int j = 0; j < y; ++j) {
      RGB& color = table[i * x + j];
      bmp.set_pixel(i, j, color.red * 255, color.green * 255, color.blue * 255);
    }
  }

  char filename[] = "raytracing00.bmp";
  bmp.save_image(filename);
  return 0;
}
