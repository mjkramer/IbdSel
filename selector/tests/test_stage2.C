{
  gROOT->ProcessLine(".L stage2_main.cc+g");
  gROOT->ProcessLine(".x tests/test_stage2_impl.C");
}
