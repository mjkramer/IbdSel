void run_stage2_impl(const char* confFile, const char* inFile, const char* outFile,
                     int site, int phase, UInt_t seq)
{
  stage2_main(confFile, inFile, outFile, Site(site), Phase(phase), seq);
}
