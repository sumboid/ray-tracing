#pragma once
#include <ts/system/System.h>
#include <ts/types/AbstractCell.h>

#include "tracing/scene.h"
#include "tracing/camera.h"
#include "tracing/light.h"
#include "tracing/objects/sphere.h"
#include "tracing/lowlevel.h"

class ReduceData: public ts::type::ReduceData {
public:
  ReduceData() {}
  virtual ~ReduceData() {}
  virtual ts::type::ReduceData* copy() { return new ReduceData; }
};

class ReduceDataTools: public ts::type::ReduceDataTools {
public:
  ~ReduceDataTools() {}
  void serialize(ts::type::ReduceData* data, char*& buf, size_t& size) {}

  ts::type::ReduceData* deserialize(void* buf, size_t size) {
    return new ReduceData;
  }

  ts::type::ReduceData* reduce(ts::type::ReduceData* d1,
                                 ts::type::ReduceData* d2) {
    return new ReduceData();
  }
};

class Cell: public ts::type::AbstractCell {
  trace::Camera* camera;
  trace::RGB* result;

public:
  Cell(ts::type::ID id, trace::Camera* _camera): ts::type::AbstractCell(id) {
    camera = _camera;
  }

  ~Cell() {
  }

  void run(std::vector<ts::type::AbstractCell*> neighbours) {
    result = camera->run();
    end();
  }

  trace::RGB* getResult() {
    return result;
  }

  ReduceData* reduce() {
    return new ReduceData();
  }

  ReduceData* reduce(ts::type::ReduceData* data) {
    return new ReduceData();
  }


  void reduceStep(ts::type::ReduceData* data) {
  }

  void update(ts::type::AbstractCell* cell) {
  }

};

class CellTools: public ts::type::AbstractCellTools {
public:
  ~CellTools() {}
  void serialize(ts::type::AbstractCell* cell, char*& buf, size_t& size) {
    buf = new char[4];
    size = 4;
  }

  ts::type::AbstractCell* deserialize(char* buf, size_t size) {
    return new Cell(ts::type::ID(0,0,0), 0);
  }
};
