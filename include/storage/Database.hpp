//
// Created by bdardenn on 5/8/20.
//

#pragma once

#include <SQLiteCpp/Database.h>
#include <string>

namespace medor::storage {

/**
 * Class that contains our DB connection settings
 */
class Database {
  public:
    Database(std::string path, int flags);

    /**
     * @return A db connection handle.
     */
    SQLite::Database handle();

  private:
    std::string _path;
    int _flags;
};

} // namespace medor::storage
