#ifndef IREADFILE_SQLITE_H
#define IREADFILE_SQLITE_H

#include <memory>

namespace irr {
namespace io {
class IReadFile;
}
}

struct sqlite3_vfs;
struct sqlite3;

std::unique_ptr<sqlite3_vfs> irrsqlite_createfilesystem();

int irrdb_open(irr::io::IReadFile* reader, sqlite3** ppDb, int flags);

#endif //IREADFILE_SQLITE_H