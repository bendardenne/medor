//
// Created by bdardenn on 4/22/20.
//

#include "storage/VcsStore.h"
#include <string>

using namespace medor;

storage::VcsStore::VcsStore(sqlite3* db_connection) : _db(db_connection) { setupDb(); }

void storage::VcsStore::setupDb() {}

void storage::VcsStore::addRepo(std::string repo, std::string project) {

}
