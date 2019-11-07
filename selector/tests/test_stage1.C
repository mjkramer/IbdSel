void test_stage1(int runno, int fileno)
{
  gSystem->Load("/usr/common/software/python/2.7-anaconda-2019.07/lib/libsqlite3.so");
  gROOT->ProcessLine(".L tests/FileFinder.cc+");
  gROOT->ProcessLine(".L stage1_main.cc+g");

  auto line = Form(".x tests/test_stage1_impl.C(%d, %d)", runno, fileno);

  gROOT->ProcessLine(line);
}
