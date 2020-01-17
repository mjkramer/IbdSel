void run_stage1(const char* inFile, const char* outFile, int site, int stage)
{
  gErrorIgnoreLevel = kError;  // suppress warnings of missing NuWa dictionaries
  gROOT->ProcessLine(".L ../selector/stage1_main.cc+");

  // We have to jump through these hoops in order to pass the Site/Stage enums
  // to stage1_main... sigh
  auto line = Form(".x cling/run_stage1_impl.C(\"%s\", \"%s\", %d, %d)", inFile, outFile, site, stage);
  gROOT->ProcessLine(line);
}
