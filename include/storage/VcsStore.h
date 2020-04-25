//
// Created by bdardenn on 4/22/20.
//

#ifndef MEDOR_VCSSTORE_H
#define MEDOR_VCSSTORE_H

#include <string>

#include <sqlite3.h>

namespace medor::storage {
class VcsStore {
  public:
    explicit VcsStore(sqlite3* db_connection);

    void addRepo(std::string repo, std::string project);

  private:
    sqlite3* _db;

    void setupDb();
};

} // namespace medor::storage

#endif // MEDOR_VCSSTORE_H
