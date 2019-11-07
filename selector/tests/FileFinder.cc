#ifndef FILEFINDER_CC
#define FILEFINDER_CC

#include <sqlite3.h>
#include <cstdlib>
#include <string>

// this shold be static, but ROOT/cling complains and fails if so
const char* g_dbfile = "/global/homes/m/mkramer/projscratch/p17b/code/p17b_find/data/p17b_files.sqlite";

static const int BUFSZ = 1024;

class FileFinder {
public:
  FileFinder()
  {
    sqlite3_open(g_dbfile, &m_db);
  }

  std::string find(int runno, int fileno)
  {
    char sql[BUFSZ];
    snprintf(sql, BUFSZ, "select path from files where runno = %d and fileno = %d", runno, fileno);

    char buf[BUFSZ] = "\0";
    sqlite3_exec(m_db, sql, callback, buf, NULL);
    return buf;
  }

private:
  sqlite3* m_db;

  static int callback(void* buf, int argc, char** argv, char** _colnames)
  {
    strncpy((char*) buf, argv[0], BUFSZ);
    return 0;
  }
};

#endif  // FILEFINDER_CC
