#pragma once

#include <vector>
#include <string>

static const std::vector<std::string> deffiles = {
 //"/global/projecta/projectdirs/dayabay/data/dropbox/p17b/lz3.skim.6/recon.Neutrino.0021221.Physics.EH1-Merged.P17B-P._0001.root"
 "inputs/dayabay_p19a_test/recon.Neutrino.0072508.Physics.EH1-Merged.P19A-I._0001.root"
};

void ibd_main(const std::vector<std::string>& inFiles = deffiles,
              const char* outFile = "results.root");
