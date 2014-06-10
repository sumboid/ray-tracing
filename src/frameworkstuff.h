#pragma once

#include <ts/types/Fragment.h>
#include <ts/types/FragmentTools.h>
#include <ts/types/ID.h>
#include <ts/types/ReduceData.h>
#include <ts/types/ReduceDataTools.h>
#include <ts/util/Uberlogger.h>
#include <ts/util/Arc.h>
#include "bitmap.h"
#include <string>

#include "tracing/scene.h"
#include "tracing/camera.h"
#include "tracing/light.h"
#include "tracing/objects/sphere.h"
#include "tracing/lowlevel.h"

using ts::type::ID;

class ReduceData: public ts::type::ReduceData {
public:
  ReduceData() {}
  virtual ~ReduceData() {}
  virtual ts::type::ReduceData* copy() { return new ReduceData(); }
};

class ReduceDataTools: public ts::type::ReduceDataTools {
public:
  ~ReduceDataTools() {}
  ts::Arc* serialize(ts::type::ReduceData*) {
    return new ts::Arc;
  }

  ts::type::ReduceData* deserialize(ts::Arc*) {
    return new ReduceData();
  }

  ts::type::ReduceData* reduce(ts::type::ReduceData*,
                                 ts::type::ReduceData*) {
    return new ReduceData();
  }
};

/* Fragment description */

class Fragment: public ts::type::Fragment {
friend class FragmentTools;
private:
  trace::Camera* camera;
  trace::RGB* result;

public:
  Fragment(ts::type::ID id, trace::Camera* _camera): ts::type::Fragment(id) {
    camera = _camera;
  }

  ~Fragment() {
  }

  void runStep(std::vector<ts::type::Fragment*>) override {
    result = camera->run();
    int dx = (camera->part[1] - camera->part[0]);
    int dy =  (camera->part[3] - camera->part[2]);

    ULOG(fragment) << "OK: " << dx << "x" << dy << UEND;
    bitmap_image bmp(dx, dy);
    for(int x = 0; x < dx; ++x) {
      for(int y = 0; y < dy; ++y) {
        trace::RGB& color = result[x * dy + y];
        bmp.set_pixel(x, y, color.red * 255, color.green * 255, color.blue * 255);
      }
    }

    bmp.save_image(std::to_string(id().c[0]) + "." + std::to_string(id().c[1]) + "." + std::to_string(id().c[2]) + ".bmp");
    delete[] result;
    delete camera;
    setEnd();
  }

  trace::RGB* getResult() {
    return result;
  }

  ReduceData* reduce() override {
    return new ReduceData();
  }

  ReduceData* reduce(ts::type::ReduceData*) override {
    return new ReduceData();
  }


  void reduceStep(ts::type::ReduceData*) override {}

  Fragment* getBoundary() override {
    Fragment* fragment = new Fragment(id(), 0);
    return fragment;
  }

  Fragment* copy() override {
    return new Fragment(id(), 0);
  }
};

class FragmentTools: public ts::type::FragmentTools {
private:
  trace::Camera* camera;
  trace::Scene* scene;
public:
  FragmentTools(trace::Scene* s, trace::Camera* c) {
    scene = s;
    camera = c;
  }

  ~FragmentTools() {}

  void bserialize(ts::type::Fragment*, ts::Arc*) {
  }

  ts::type::Fragment* bdeserialize(ts::Arc*) {
    Fragment* result = new Fragment(ts::type::ID(0, 0, 0), 0);
    return result;
  }

  void fserialize(ts::type::Fragment* fragment, ts::Arc* arc) {
    ts::Arc& a = *arc;
    Fragment* f = (Fragment*) fragment;
    a << f->camera->part[0] << f->camera->part[1] << f->camera->part[2] << f->camera->part[3];
  }

  ts::type::Fragment* fdeserialize(ts::Arc* arc) {
    ts::Arc& a = *arc;
    camera->setScene(scene);
    int part[4];
    a >> part[0];
    a >> part[1];
    a >> part[2];
    a >> part[3];
    camera->setPart(part[0], part[2], part[1], part[3]);
    Fragment* result = new Fragment(ts::type::ID(0, 0, 0), camera);
    return result;
  }

  ts::type::Fragment* createGap(const ID& id) override {
    return new Fragment(id, 0);
  }
};


