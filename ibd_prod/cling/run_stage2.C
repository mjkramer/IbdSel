void run_stage2(const char* confFile, const char* inFile, const char* outFile,
                int site, int phase, UInt_t seq)
{
  gROOT->ProcessLine(".L ../selector/stage2_main.cc+");

  auto line = Form(".x cling/run_stage2_impl.C(\"%s\", \"%s\", \"%s\", %d, %d, %d)",
                   confFile, inFile, outFile, site, phase, seq);
  gROOT->ProcessLine(line);
}
