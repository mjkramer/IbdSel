void run_stage2(const char* confFile, const char* inFile, const char* outFile,
                int site, int phase, UInt_t seq)
{
  gSystem->Load("../selector/stage2_main_cc.so");

  auto form = "stage2_main(\"%s\", \"%s\", \"%s\", Site(%d), Phase(%d), %d)";
  auto line = Form(form, confFile, inFile, outFile, site, phase, seq);
  gROOT->ProcessLine(line);
}
