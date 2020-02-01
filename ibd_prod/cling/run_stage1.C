void run_stage1(const char* inFile, const char* outFile, int site, int phase)
{
  gErrorIgnoreLevel = kError;  // suppress warnings of missing NuWa dictionaries

  gROOT->ProcessLine(".L ../selector/stage1_main.cc+");

  auto form = "stage1_main(\"%s\", \"%s\", Site(%d), Phase(%d))";
  auto line = Form(form, inFile, outFile, site, phase);
  gROOT->ProcessLine(line);
}
