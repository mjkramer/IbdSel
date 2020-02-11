void test_stage1(int runno, int fileno, int site, int phase)
{
  gErrorIgnoreLevel = kError;  // suppress warnings of missing NuWa dictionaries

  gSystem->Load("/usr/common/software/python/2.7-anaconda-2019.07/lib/libsqlite3.so");
  gROOT->ProcessLine(".L tests/FileFinder.cc+");

  gROOT->ProcessLine(".x cling/Build.C(\"stage1/stage1_main.cc\")");

  auto line = Form(".x tests/test_stage1_impl.C(%d, %d, %d, %d)", runno, fileno, site, phase);

  gROOT->ProcessLine(line);
}
