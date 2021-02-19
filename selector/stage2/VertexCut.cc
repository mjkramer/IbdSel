#include "VertexCut.hh"

#include "../SelectorFramework/core/ConfigTool.hh"

#include <cmath>

void VertexCutTool::connect(Pipeline& pipeline)
{
  auto config = pipeline.getTool<Config>();

  cuts.minZ = config->get<float>("minZ", -10'000);
  cuts.maxZ = config->get<float>("maxZ", 10'000);
  cuts.minR = config->get<float>("minR", 0);
  cuts.maxR = config->get<float>("maxR", 10'000);
}

bool VertexCutTool::vertexOk(Iter event) const
{
  const float z = event->z;
  const float r = sqrt(event->x * event->x + event->y * event->y);

  const bool zOk = cuts.minZ <= z && z <= cuts.maxZ;
  const bool rOk = cuts.minR <= r && r <= cuts.maxR;

  return zOk && rOk;
}
