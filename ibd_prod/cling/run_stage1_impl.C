void run_stage1_impl(const char* inFile, const char* outFile, int site, int phase)
{
  stage1_main(inFile, outFile, Site(site), Phase(phase));
}
