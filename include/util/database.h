//
// Created by bdardenn on 4/22/20.
//

#ifndef MEDOR_DATABASE_H
#define MEDOR_DATABASE_H

#include <sqlite3.h>

namespace medor::util::database {

void createTables(sqlite3* db_connection);

void checkError(int ret);

} // namespace medor::util::database

#endif // MEDOR_DATABASE_H
