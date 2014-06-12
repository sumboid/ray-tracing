#pragma once

#include <ts/types/Fragment.h>
#include <ts/types/FragmentTools.h>
#include <ts/types/ID.h>
#include <ts/types/ReduceData.h>
#include <ts/types/ReduceDataTools.h>
#include <ts/util/Uberlogger.h>
#include <ts/util/Arc.h>
#include <algorithm>
#include "bitmap.h"
#include <string>
#include <cstring>
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
  trace::RGB* result = 0;
  trace::RGB* r = 0;
  size_t rs;

public:
  Fragment(ts::type::ID id, trace::Camera* _camera): ts::type::Fragment(id) {
    if(id == ID(-1, -1, -1)) {
      ULOG(error) << "OK" << UEND;
      setNeighbours(0, 0);
    } else {
      camera = _camera;
    }
  }

  ~Fragment() {
    if(result != 0) delete[] result;
    if(r != 0) delete[] r;
    if(camera != 0) delete camera;
  }

  void runStep(std::vector<ts::type::Fragment*> fs) override {
    if(id() == ID(-1, -1, -1)) {
      int size = 500;

      bitmap_image bmp(size, size);

      std::map<uint64_t, std::vector<Fragment*>> sfs;

      for(auto f : fs) {
        sfs[f->id().c[0]].push_back((Fragment*) f);
      }

      std::map<uint64_t, Fragment**> rfs;
      size_t* sizes = new size_t[sfs.size()];

      for(auto& sf : sfs) {
        rfs[sf.first] = new Fragment*[sf.second.size()];
        sizes[sf.first] = sf.second.size();
        for(auto f: sf.second) {
          rfs[sf.first][f->id().c[1]] = f;
        }
      }

      int ry = 0;
      for(size_t i = 0; i < rfs.size(); ++i) {
        for(size_t j = 0; j < sizes[i]; ++j) {
          Fragment* f = rfs[i][j];
          int lines = f->rs / size;

          for(int y = 0; y < lines; ++y) {
            for(int x = 0; x < size; ++x) {
              trace::RGB color = f->r[y * size + x];
              bmp.set_pixel(x, ry, color.red * 255, color.green * 255, color.blue * 255);
            }
            ++ry;
          }
        }
        delete[] rfs[i];
      }

      delete[] sizes;

      bmp.save_image("result.bmp");
      ULOG(success) << "Picture is done" << UEND;
      setEnd();
    }
    else {
      result = camera->run();
      int sizex = 500;
      int dx = (camera->part[1] - camera->part[0]);
      int dy =  (camera->part[3] - camera->part[2]);

      int delta = dx / sizex;
      int sizey = dy / delta;

      rs = sizex * sizey;
      r = new trace::RGB[rs];

      for(int x = 0; x < sizex; x++) {
        for(int y = 0; y < sizey; y++) {
          int rx = (x) * delta + delta / 2;
          int ry = (y) * delta + delta / 2;

          if(rx >= dx) rx = dx - 1;
          if(ry >= dy) ry = dy - 1;

          r[y * sizex + x] = result[ry * dx + rx];
          for(int j = ry - delta / 2; j <= ry + delta / 2; ++j)
            for(int i = rx - delta / 2; i <= rx + delta / 2; ++i) {
              if(j >= dy || i >= dx) continue;
              if(j != ry && i != rx) r[y * sizex + x] = r[y * sizex + x].realmix(result[j * dx + i]);
            }
        }
      }
      saveState();
      setUpdate();
      setEnd();
    }
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
    fragment->rs = rs;
    fragment->r = new trace::RGB[rs];
    memcpy(fragment->r, r, rs * sizeof(trace::RGB));
    return fragment;
  }

  Fragment* copy() override {
    return new Fragment(id(), 0);
  }

  uint64_t weight() {
    if(id() == ID(-1, -1, -1)) return 0;
    return (camera->part[1] - camera->part[0]) * (camera->part[3] - camera->part[2]);
  }
};

class FragmentTools: public ts::type::FragmentTools {
friend class Fragment;
private:
  trace::Camera* camera;
  trace::Scene* scene;
public:
  FragmentTools(trace::Scene* s, trace::Camera* c) {
    scene = s;
    camera = c;
  }

  ~FragmentTools() {}

  void bserialize(ts::type::Fragment* fragment, ts::Arc* arc) {
    ts::Arc& a = *arc;
    Fragment* f = (Fragment*) fragment;
    a << f->rs;
    for(size_t i = 0; i < f->rs; ++i) {
      double r = f->r[i].red;
      double g = f->r[i].green;
      double b = f->r[i].blue;
      a << r << g << b;
    }
  }

  ts::type::Fragment* bdeserialize(ts::Arc* arc) {
    Fragment* result = new Fragment(ts::type::ID(0, 0, 0), 0);
    ts::Arc& a = *arc;
    a >> result->rs;
    result->r = new trace::RGB[result->rs];
    for(size_t i = 0; i < result->rs; ++i) {
      double r, g, b;
      a >> r >> g >> b;

      result->r[i] = trace::RGB(r,g,b);
    }

    return result;
  }

  void fserialize(ts::type::Fragment* fragment, ts::Arc* arc) {
    ts::Arc& a = *arc;
    Fragment* f = (Fragment*) fragment;
    a << f->camera->part[0] << f->camera->part[1] << f->camera->part[2] << f->camera->part[3];
  }

  ts::type::Fragment* fdeserialize(ts::Arc* arc) {
    ts::Arc& a = *arc;
    int part[4];
    a >> part[0];
    a >> part[1];
    a >> part[2];
    a >> part[3];
    camera->setPart(part[0], part[2], part[1], part[3]);
    Fragment* result = new Fragment(ts::type::ID(0, 0, 0), camera->copy());
    return result;
  }

  ts::type::Fragment* createGap(const ID& id) override {
    return new Fragment(id, 0);
  }
};


