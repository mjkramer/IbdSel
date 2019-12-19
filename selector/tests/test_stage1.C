void test_stage1(int runno, int fileno, int site, int stage)
{
  const char* debug = getenv("IBDSEL_DEBUG") ? "g" : "";
  gROOT->ProcessLine(".x LoadBoost.C");
  gSystem->Load("/usr/common/software/python/2.7-anaconda-2019.07/lib/libsqlite3.so");
  gROOT->ProcessLine(".L tests/FileFinder.cc+");
  gROOT->ProcessLine(Form(".L stage1_main.cc+%s", debug));

  auto line = Form(".x tests/test_stage1_impl.C(%d, %d, %d, %d)", runno, fileno, site, stage);

  gROOT->ProcessLine(line);
}
