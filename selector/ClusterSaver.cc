#include "ClusterSaver.hh"

ClusterSaver& makeClusterSaver(Pipeline& p, int d){
  return p.makeAlg<ClusterSaver>(d);
}

