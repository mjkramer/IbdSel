#include "stage1_main.hh"

#include <TError.h>

#include <iostream>
#include <string>

int main(int argc, char** argv)
{
  gErrorIgnoreLevel = kError;  // suppress warnings of missing NuWa dictionaries

  if (argc != 4 + 1) {
    std::cerr << "Usage: " << argv[0] << " infile outfile site phase\n";
    return 1;
  }

  const auto inFile = argv[1];
  const auto outFile = argv[2];
  const auto site = Site(std::stoul(argv[3]));
  const auto phase = Phase(std::stoul(argv[4]));

  stage1_main(inFile, outFile, site, phase);

  return 0;
}
