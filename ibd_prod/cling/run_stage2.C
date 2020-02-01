void run_stage2(const char* confFile, const char* inFile, const char* outFile,
                int site, int phase, UInt_t seq)
{
  gROOT->ProcessLine(".L ../selector/stage2_main.cc+");

  auto form = "stage2_main(\"%s\", \"%s\", \"%s\", Site(%d), Phase(%d), %d)";
  auto line = Form(form, confFile, inFile, outFile, site, phase, seq);
  gROOT->ProcessLine(line);
}
