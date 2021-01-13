#ifndef IREADFILE_SQLITE_H
#define IREADFILE_SQLITE_H

struct sqlite3;

namespace irr {
namespace io {
class IReadFile;
}
}

int readonlymemvfs_init();

int irrdb_open(irr::io::IReadFile* reader, sqlite3** ppDb, int flags);

#endif //IREADFILE_SQLITE_H