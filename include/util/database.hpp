//
// Created by bdardenn on 4/22/20.
//
#pragma once

#include <SQLiteCpp/Database.h>

namespace medor::util::database {

void createTables(SQLite::Database& dbConnection);

void checkError(int ret);

} // namespace medor::util::database
