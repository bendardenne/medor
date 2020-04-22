//
// Created by bdardenn on 4/22/20.
//

#include <string>

#include <util/database.h>
#include <iostream>

using namespace medor;

void util::database::createTables(sqlite3 *db_connection) {
    char *error = 0;
    std::string sql = R"sql(
    create table if not exists activities (
        id integer primary key autoincrement,
        start text not null,
        end text not null,
        comment text not null,
        project_id integer not null
    );)sql";

    int ret = sqlite3_exec(db_connection, sql.c_str(), nullptr, 0, &error);
    util::database::checkError(ret);

    sql = R"sql(
        create table if not exists projects (
                id integer primary key autoincrement,
                name text not null
          );)sql";

    ret = sqlite3_exec(db_connection, sql.c_str(), nullptr, 0, &error);
    util::database::checkError(ret);

    sql = R"sql(
        create table if not exists repos (
                id integer primary key autoincrement,
                path text not null,
                project_id integer
          );)sql";

    ret = sqlite3_exec(db_connection, sql.c_str(), nullptr, 0, &error);
    util::database::checkError(ret);
}

void inline util::database::checkError(int errorCode) {
    if (errorCode == SQLITE_OK) {
        return;
    }

    std::string errorMessage = sqlite3_errstr(errorCode);
    std::cerr << errorMessage << std::endl;
}


