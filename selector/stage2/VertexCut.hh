#pragma once

#include "AdBuffer.hh"

class VertexCutTool : public Tool {
public:
  using Iter = AdBuffer::Iter;

  void connect(Pipeline& pipeline) override;

  bool vertexOk(Iter event) const;

  struct Cuts {
    float minZ, maxZ, minR, maxR;
  };

  Cuts cuts;
};
