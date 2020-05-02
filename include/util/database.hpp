//
// Created by bdardenn on 4/22/20.
//
#pragma once

#include <sqlite3.h>

namespace medor::util::database {

void createTables(sqlite3* dbConnection);

void checkError(int ret);

} // namespace medor::util::database
