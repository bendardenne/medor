//
// Created by bdardenn on 5/8/20.
//
#include "storage/Database.hpp"
#include "util/database.hpp"

SQLite::Database medor::storage::Database::handle() { return SQLite::Database(_path, _flags); }

medor::storage::Database::Database(std::string path, int flags) : _path(std::move(path)), _flags(flags) {
    SQLite::Database handle = this->handle();

    if ((flags & SQLite::OPEN_READWRITE) != 0) {
        util::database::createTables(handle);
    }
}
