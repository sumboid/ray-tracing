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
  for(int ix = part[0]; ix < part[1]; ix++)
    for(int iz = part[2]; iz < part[3]; iz++)
    {
      Vector ray (ix*imagePlaneSizeX/imagePlaneResolutionX-imagePlaneSizeX/2,
                  imagePlaneDistance,
                  iz*imagePlaneSizeZ/imagePlaneResolutionZ-imagePlaneSizeZ/2);

      ray = ray.norm();

      RGB color = scene->getColor(vp, ray);

      if(color.red > 1)
        color.red = 1;
      if(color.blue > 1)
        color.blue = 1;
      if(color.green > 1)
        color.green = 1;

      table[ix * (part[1] - part[0]) + iz] = color;
    }
  return table;
}
}
