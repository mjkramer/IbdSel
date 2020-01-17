void run_stage1_impl(const char* inFile, const char* outFile, int site, int stage)
{
  stage1_main(inFile, outFile, Site(site), Stage(stage));
}
