#include "AdTree.hh"

void AdTree::initBranches()
{
  BR(trigSec);
  BR(trigNanoSec);
  BR(energy);
  BR(x); BR(y); BR(z);

  BR(trigNo);
  BR(runNo);
  BR(fileNo);
}
