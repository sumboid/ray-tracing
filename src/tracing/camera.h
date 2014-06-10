#pragma once
#include "lowlevel.h"
#include "scene.h"

namespace trace {
class Camera {
public:
  double backgroundSizeX;
  double backgroundSizeZ;
  double backgroundDistance;

  double imagePlaneDistance;
  double imagePlaneSizeX;
  double imagePlaneSizeZ;

  int imagePlaneResolutionX;
  int imagePlaneResolutionZ;

  int part[4];

  Point vp;
  Scene* scene;
public:
  Camera(double bsx, double bsz, double bd, double ipd);

  void setResolution(int x, int y);
  void setPart(int ulx, int uly, int drx, int dry);
  void setViewPoint(const Point& p);
  void setScene(Scene* _scene);

  RGB* run();
};

}
