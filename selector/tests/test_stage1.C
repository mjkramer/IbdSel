{
  gROOT->ProcessLine(".L stage1_main.cc+g");
  gROOT->ProcessLine(".x tests/test_stage1_impl.C");
}
