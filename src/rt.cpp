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

  Point sub(Point another) {
    return Point(another.x - x, another.y - y, another.z - z);
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

  Vector() {
    undefined = true;
  }

  bool isDefined() { return !undefined; }

  static Vector get(const Point& x, const Point& y) {
    return x.sub(y);
  }

  unsigned double mod() {
    return x * x + y * y + z * z;
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
};

class Object {
public:
  virtual Object() {}
  virtual Vector interspect(const Vector& start, const Vector& ray) = 0;
};

class Sphere : public Object {
private:
  Point p;
  double r;
  RGB color;

public:
  Sphere(const Point& point, double _r, const RGB& _color) {
    p = point;
    r = _r;
    color = _color;
  }

  virtual Point interspect(const Vector& start, const Vector& ray) {
    auto v = Vector::get(start, p);

    double d = (v.x * ray.x + v.y * ray.y + v.z * ray.z) *
               (v.x * ray.x + v.y * ray.y + v.z * ray.z) -
               ((v.x * v.x + v.y * v.y + v.z * v.z) - r * r);
    if(d < 0)
      return Point();

    double t1 = -(v.x * ray.x + v.y * ray.y + v.z * ray.z) + ::sqrt(d);
    double t2 = -(v.x * ray.x + v.y * ray.y + v.z * ray.z) - ::sqrt(d);

    double t = 0;

    if(t1 < std::epsilon())
    {
      if(t2 < std::epsilon())
        return Point();
      else
        t = t2;
    }
    else
      if(t2 < std::epsilon())
        t = t1;
      else
        t = (t1 > t2) ? t2 : t1;

    return Point(ray.x * t + start.x,
                 ray.y * t + start.y,
                 ray.z * t + start.z);

  }
};

struct Light {
  Point p;
  RGB color;

  Light(const Point& point, const RGB& _color) {
    p = point;
    color = _color;
  }
};

class Scene {
private:
  std::vector<Object*> objects;
  std::vector<Light*> lights;

public:
  Scene() {}
  ~Scene() {}

  void addObject(Object* o) { objects.push_back(o); }
  void addLight(Light* l) { lights.push_back(l); }

  Point* intersect(const Vector& start, const Vector& ray) {
    Object* min  = *std::min_element(objects.begin(), objects.end(),
                                     [&start, &ray](Object* x, Object* y) {
      Point interspectX = x->interspect(start, ray);
      Point interspectY = y->interspect(start, ray);
      if(!interspectX.isDefined()) return false;
      if(!interspectY.isDefined()) return true;

      return Vector::get(interspectX, start).mod() < Vector::get(interspectY, start);
    });

    if(min->interspect(start, ray).isDefined()) {
      return min;
    }
    else {
      return 0;
    }
  }
  RGB illumination(const Vector& start, const Vector& ray)
  {
    Object* object = intersect(start, ray);
    if(object == 0) { // Not cool, but who cares
      return RGB(0, 0, 0);
    }

    Point point = object->interspect(start, ray);

    RGB color(0, 0, 0);

    for(int i = 0; i<sizeOfLOLS; i++)
    {
      //check if the point on the object is illuminated
      Vector rayToLightSource;
      rayToLightSource.x = listOfLightSources[i].x-point.x;
      rayToLightSource.y = listOfLightSources[i].y-point.y;
      rayToLightSource.z = listOfLightSources[i].z-point.z;

      double rayToLightSourceLenght = sqrt(rayToLightSource.x*rayToLightSource.x
                                           +rayToLightSource.y*rayToLightSource.y
                                           +rayToLightSource.z*rayToLightSource.z
                                           );

      rayToLightSource.x /= rayToLightSourceLenght;
      rayToLightSource.y /= rayToLightSourceLenght;
      rayToLightSource.z /= rayToLightSourceLenght;

      Vector intersectionPoint;

      //optimization potential: do not need closest object here, just check for an obstacle
      int obstacle = intersect(point, rayToLightSource, intersectionPoint);

      if(obstacle!=-1)
      {
        //check if light is closer then the intersected object
        intersectionPoint.x -= point.x;
        intersectionPoint.y -= point.y;
        intersectionPoint.z -= point.z;

        double obstacleDistance = sqrt(intersectionPoint.x*intersectionPoint.x
                                       +intersectionPoint.y*intersectionPoint.y
                                       +intersectionPoint.z*intersectionPoint.z);

        if(obstacleDistance>rayToLightSourceLenght)
        {
          obstacle=-1;
        }
      }

      if(obstacle==-1)
      {
        //reflection
        Vector normal = {
          point.x-listOfSpheres[object].x,
          point.y-listOfSpheres[object].y,
          point.z-listOfSpheres[object].z
        };
        double norm = sqrt(normal.x*normal.x+normal.y*normal.y+normal.z*normal.z);
        normal.x /= norm;
        normal.y /= norm;
        normal.z /= norm;
        double cosine = normal.x*(-ray.x)+normal.y*(-ray.y)+normal.z*(-ray.z);
        Vector reflectedRay = {ray.x+2*cosine*normal.x,
                               ray.y+2*cosine*normal.y,
                               ray.z+2*cosine*normal.z
                              };
        cosine = reflectedRay.x*rayToLightSource.x+reflectedRay.y*rayToLightSource.y+reflectedRay.z*rayToLightSource.z;

        if(cosine<0)
          cosine = 0;

        // cosine = 1;

        //apply coefficients of the body color to the intensity of the light source
        color.red += listOfLightSources[0].color.red*listOfSpheres[object].color.red*cosine;
        color.blue += listOfLightSources[0].color.blue*listOfSpheres[object].color.blue*cosine;
        color.green += listOfLightSources[0].color.green*listOfSpheres[object].color.green*cosine;

      }
    }
    //apply ambient light
    color.red += ambientLight.red*listOfSpheres[object].color.red;
    color.blue += ambientLight.blue*listOfSpheres[object].color.blue;
    color.green += ambientLight.green*listOfSpheres[object].color.green;



    if(recursionStep!=limitOfRecursion)
    {

      //reflection
      Vector normal = { point.x-listOfSpheres[object].x,
                        point.y-listOfSpheres[object].y,
                        point.z-listOfSpheres[object].z
                      };
      double norm = sqrt(normal.x*normal.x+normal.y*normal.y+normal.z*normal.z);
      normal.x /= norm;
      normal.y /= norm;
      normal.z /= norm;
      double cosine = normal.x*(-ray.x)+normal.y*(-ray.y)+normal.z*(-ray.z);
      Vector reflectedRay = {(ray.x+2*cosine*normal.x),
                             (ray.y+2*cosine*normal.y),
                             (ray.z+2*cosine*normal.z)
                            };

      //the followin norming can have no sense
      norm = sqrt(reflectedRay.x*reflectedRay.x+reflectedRay.y*reflectedRay.y+reflectedRay.z*reflectedRay.z);
      reflectedRay.x /= norm;
      reflectedRay.y /= norm;
      reflectedRay.z /= norm;


      RGB reflectionColor = illuminationStep(point, reflectedRay, recursionStep+1);
      color.red += listOfSpheres[object].color.red*reflectionColor.red;
      color.blue += listOfSpheres[object].color.blue*reflectionColor.blue;
      color.green += listOfSpheres[object].color.green*reflectionColor.green;
    }

    return color;
  }

};

const Vector zeroPoint = {0, 0, 0};
RGB redPixel = {0.3, 0.3, 1};
RGB bluePixel = {1, 0.3, 0.3};
RGB greenPixel = {0.3, 1, 0.3};
const RGB blackPixel = {0, 0, 0};
RGB somePixel = {0.6, 0.2, 0.2};
RGB whitePixel = {0.5, 0.5, 0.5};
RGB ambientLight = {0.1, 0.1, 0.1};
RGB yellowPixel = {0.3, 1 , 1};


const int limitOfRecursion = 50;

RGB illuminationStep(Vector point, Vector ray, int recursionStep = 0)
{

  Vector closestIntersectionPoint;
  //find an object we a lookint at
  int closestObject = intersect(point, ray, closestIntersectionPoint);

  if(closestObject==-1)
    return blackPixel;


  RGB color = blackPixel;

  for(int i = 0; i<sizeOfLOLS; i++)
  {
    //check if the point on the object is illuminated
    Vector rayToLightSource;
    rayToLightSource.x = listOfLightSources[i].x-closestIntersectionPoint.x;
    rayToLightSource.y = listOfLightSources[i].y-closestIntersectionPoint.y;
    rayToLightSource.z = listOfLightSources[i].z-closestIntersectionPoint.z;

    double rayToLightSourceLenght = sqrt(rayToLightSource.x*rayToLightSource.x
                                         +rayToLightSource.y*rayToLightSource.y
                                         +rayToLightSource.z*rayToLightSource.z
                                         );

    rayToLightSource.x /= rayToLightSourceLenght;
    rayToLightSource.y /= rayToLightSourceLenght;
    rayToLightSource.z /= rayToLightSourceLenght;

    Vector intersectionPoint;

    //optimization potential: do not need closest object here, just check for an obstacle
    int obstacle = intersect(closestIntersectionPoint, rayToLightSource, intersectionPoint);

    if(obstacle!=-1)
    {
      //check if light is closer then the intersected object
      intersectionPoint.x -= closestIntersectionPoint.x;
      intersectionPoint.y -= closestIntersectionPoint.y;
      intersectionPoint.z -= closestIntersectionPoint.z;

      double obstacleDistance = sqrt(intersectionPoint.x*intersectionPoint.x
                                     +intersectionPoint.y*intersectionPoint.y
                                     +intersectionPoint.z*intersectionPoint.z);

      if(obstacleDistance>rayToLightSourceLenght)
      {
        obstacle=-1;
      }
    }

    if(obstacle==-1)
    {
      //reflection
      Vector normal = {
        closestIntersectionPoint.x-listOfSpheres[closestObject].x,
        closestIntersectionPoint.y-listOfSpheres[closestObject].y,
        closestIntersectionPoint.z-listOfSpheres[closestObject].z
      };
      double norm = sqrt(normal.x*normal.x+normal.y*normal.y+normal.z*normal.z);
      normal.x /= norm;
      normal.y /= norm;
      normal.z /= norm;
      double cosine = normal.x*(-ray.x)+normal.y*(-ray.y)+normal.z*(-ray.z);
      Vector reflectedRay = {ray.x+2*cosine*normal.x,
                             ray.y+2*cosine*normal.y,
                             ray.z+2*cosine*normal.z
                            };
      cosine = reflectedRay.x*rayToLightSource.x+reflectedRay.y*rayToLightSource.y+reflectedRay.z*rayToLightSource.z;

      if(cosine<0)
        cosine = 0;

      // cosine = 1;

      //apply coefficients of the body color to the intensity of the light source
      color.red += listOfLightSources[0].color.red*listOfSpheres[closestObject].color.red*cosine;
      color.blue += listOfLightSources[0].color.blue*listOfSpheres[closestObject].color.blue*cosine;
      color.green += listOfLightSources[0].color.green*listOfSpheres[closestObject].color.green*cosine;

    }
  }
  //apply ambient light
  color.red += ambientLight.red*listOfSpheres[closestObject].color.red;
  color.blue += ambientLight.blue*listOfSpheres[closestObject].color.blue;
  color.green += ambientLight.green*listOfSpheres[closestObject].color.green;



  if(recursionStep!=limitOfRecursion)
  {

    //reflection
    Vector normal = { closestIntersectionPoint.x-listOfSpheres[closestObject].x,
                      closestIntersectionPoint.y-listOfSpheres[closestObject].y,
                      closestIntersectionPoint.z-listOfSpheres[closestObject].z
                    };
    double norm = sqrt(normal.x*normal.x+normal.y*normal.y+normal.z*normal.z);
    normal.x /= norm;
    normal.y /= norm;
    normal.z /= norm;
    double cosine = normal.x*(-ray.x)+normal.y*(-ray.y)+normal.z*(-ray.z);
    Vector reflectedRay = {(ray.x+2*cosine*normal.x),
                           (ray.y+2*cosine*normal.y),
                           (ray.z+2*cosine*normal.z)
                          };

    //the followin norming can have no sense
    norm = sqrt(reflectedRay.x*reflectedRay.x+reflectedRay.y*reflectedRay.y+reflectedRay.z*reflectedRay.z);
    reflectedRay.x /= norm;
    reflectedRay.y /= norm;
    reflectedRay.z /= norm;


    RGB reflectionColor = illuminationStep(closestIntersectionPoint, reflectedRay, recursionStep+1);
    color.red += listOfSpheres[closestObject].color.red*reflectionColor.red;
    color.blue += listOfSpheres[closestObject].color.blue*reflectionColor.blue;
    color.green += listOfSpheres[closestObject].color.green*reflectionColor.green;
  }

  return color;
}

double backgroundSizeX = 4;
double backgroundSizeZ = 4;
double backgroundDistance = 15;

double imagePlaneDistance = 5;
double imagePlaneSizeX;
double imagePlaneSizeZ;

int imagePlaneResolutionX =600;
int imagePlaneResolutionZ = 600;

int colorDepth = 24;

int main()
{

  bitmap_image bmp(imagePlaneResolutionX, imagePlaneResolutionZ);

  listOfSpheres[4].x = 0;
  listOfSpheres[4].y = 7;
  listOfSpheres[4].z = 2;
  listOfSpheres[4].r = 1;
  listOfSpheres[4].color = bluePixel;

  listOfSpheres[3].x = -3;
  listOfSpheres[3].y = 11;
  listOfSpheres[3].z = -2;
  listOfSpheres[3].r = 2;
  listOfSpheres[3].color = redPixel;


  listOfSpheres[0].x = 0;
  listOfSpheres[0].y = 8;
  listOfSpheres[0].z = -2;
  listOfSpheres[0].r = 1;
  listOfSpheres[0].color = greenPixel;

  listOfSpheres[2].x = 1.5;
  listOfSpheres[2].y = 7;
  listOfSpheres[2].z = 0.5;
  listOfSpheres[2].r = 1;
  listOfSpheres[2].color = whitePixel;

  listOfSpheres[5].x = -2;
  listOfSpheres[5].y = 6;
  listOfSpheres[5].z = 1;
  listOfSpheres[5].r = 0.7;
  listOfSpheres[5].color = yellowPixel;


  listOfSpheres[1].x = 2.2;
  listOfSpheres[1].y = 8;
  listOfSpheres[1].z = 0.;
  listOfSpheres[1].r = 1;
  listOfSpheres[1].color = whitePixel;



  listOfSpheres[6].x = 4;
  listOfSpheres[6].y = 10;
  listOfSpheres[6].z = 1;
  listOfSpheres[6].r = 0.7;
  listOfSpheres[6].color = redPixel;


  listOfLightSources[0].x = -15;
  listOfLightSources[0].y = -15;
  listOfLightSources[0].z = 0;
  listOfLightSources[0].color = whitePixel;

  listOfLightSources[1].x = 1;
  listOfLightSources[1].y = 0;
  listOfLightSources[1].z = 1;
  listOfLightSources[1].color = bluePixel;

  listOfLightSources[2].x = 0;
  listOfLightSources[2].y = 6;
  listOfLightSources[2].z = -10;
  listOfLightSources[2].color = redPixel;



  Vector viewPoint = zeroPoint;

  //for(int i = 0; i<20; i++)
  {
    viewPoint.y = -20;//*cos(i*3.14/20);
    viewPoint.x = 0;//*sin(i*3.14/20);


    sizeOfLOS = 7;
    sizeOfLOLS = 3;

    imagePlaneSizeX = backgroundSizeX*imagePlaneDistance/backgroundDistance;
    imagePlaneSizeZ = backgroundSizeZ*imagePlaneDistance/backgroundDistance;

    for(int ix = 0; ix < imagePlaneResolutionX; ix++)
      for(int iz = 0; iz < imagePlaneResolutionZ; iz++)
      {
        //    int ix = 0, iz = 0;
        //a ray from an eye
        Vector ray;
        ray.x = ix*imagePlaneSizeX/imagePlaneResolutionX-imagePlaneSizeX/2;
        ray.z = iz*imagePlaneSizeZ/imagePlaneResolutionZ-imagePlaneSizeZ/2;
        ray.y = imagePlaneDistance;

        //    ray.y = 5;//cos(i*3.14/20);
        //    ray.x = -.1;//sin(i*3.14/20);
        //    ray.z = 0;

        double rayLength = sqrt(ray.x*ray.x+ray.y*ray.y+ray.z*ray.z);
        ray.x /= rayLength;
        ray.y /= rayLength;
        ray.z /= rayLength;


        RGB color = blackPixel;
        //std::cerr << "start\n";
        color = illuminationStep(viewPoint, ray);

        if(color.red>1)
          color.red = 1;
        if(color.blue>1)
          color.blue = 1;
        if(color.green>1)
          color.green = 1;

        bmp.set_pixel(ix, iz, 255*color.red, 255*color.green, 255*color.blue);
      }

    char filename[] = "raytracing00.bmp";
    //    filename[10] += i/10;
    //    filename[11] += i%10;
    bmp.save_image(filename);
  }
  return 0;
}
