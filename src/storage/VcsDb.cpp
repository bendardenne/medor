//
// Created by bdardenn on 4/22/20.
//

#include "storage/VcsDb.h"

using namespace medor;

storage::VcsDb::VcsDb(sqlite3 *db_connection) : _db(db_connection) {
    setupDb();
}

void storage::VcsDb::setupDb() {

}
