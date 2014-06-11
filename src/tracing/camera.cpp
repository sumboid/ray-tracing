#include <iostream>
#include <cassert>
#include "camera.h"

namespace trace {
Camera::Camera(double bsx, double bsz, double bd, double ipd) {
  backgroundSizeX = bsx;
  backgroundSizeZ = bsz;
  backgroundDistance = bd;

  imagePlaneDistance = ipd;
  imagePlaneResolutionX = 100;
  imagePlaneResolutionZ = 100;

  imagePlaneSizeX = backgroundSizeX * imagePlaneDistance / backgroundDistance;
  imagePlaneSizeZ = backgroundSizeZ * imagePlaneDistance / backgroundDistance;

  vp = Point(0, 0, 0);
}

void Camera::setResolution(int x, int y) {
  imagePlaneResolutionX = x;
  imagePlaneResolutionZ = y;

  part[0] = 0;
  part[1] = x;
  part[2] = 0;
  part[3] = y;
}

void Camera::setPart(int ulx, int uly, int drx, int dry) {
  part[0] = ulx;
  part[1] = drx;
  part[2] = uly;
  part[3] = dry;
}

void Camera::setViewPoint(const Point& p) {
  vp = p;
}

void Camera::setScene(Scene* _scene) {
  scene = _scene;
}

RGB* Camera::run() {
  RGB* table = new RGB[(part[1] - part[0]) * (part[3] - part[2])];
  for(int iy = part[2]; iy < part[3]; iy++)
    for(int ix = part[0]; ix < part[1]; ix++)
    {
      Vector ray (ix*imagePlaneSizeX/imagePlaneResolutionX-imagePlaneSizeX/2,
                  imagePlaneDistance,
                  iy*imagePlaneSizeZ/imagePlaneResolutionZ-imagePlaneSizeZ/2);

      ray = ray.norm();

      RGB color = scene->getColor(vp, ray);

      if(color.red > 1)
        color.red = 1;
      if(color.blue > 1)
        color.blue = 1;
      if(color.green > 1)
        color.green = 1;

      table[(iy - part[2]) * (part[1] - part[0]) + (ix - part[0])] = color;
    }
  return table;
}

Camera* Camera::copy() {
  Camera* c = new Camera(backgroundSizeX, backgroundSizeZ, backgroundDistance, imagePlaneDistance);
  c->vp = vp;
  c->imagePlaneResolutionX = imagePlaneResolutionX;
  c->imagePlaneResolutionZ = imagePlaneResolutionZ;
  c->part[0] = part[0];
  c->part[1] = part[1];
  c->part[2] = part[2];
  c->part[3] = part[3];
  c->scene = scene;
  return c;
}

}
