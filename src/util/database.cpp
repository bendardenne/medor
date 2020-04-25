//
// Created by bdardenn on 4/22/20.
//

#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>
#include <iostream>
#include <string>

#include "util/database.h"
#include "util/logging.h"

using namespace medor;

void util::database::createTables(sqlite3* dbConnection) {
    char* error = 0;
    std::string sql = R"sql(
    create table if not exists activities (
        id integer primary key autoincrement,
        start text not null,
        end text not null,
        comment text not null,
        project_id integer not null
    );)sql";

    int ret = sqlite3_exec(dbConnection, sql.c_str(), nullptr, 0, &error);
    util::database::checkError(ret);

    sql = R"sql(
        create table if not exists projects (
                id integer primary key autoincrement,
                name text not null
          );)sql";

    ret = sqlite3_exec(dbConnection, sql.c_str(), nullptr, 0, &error);
    util::database::checkError(ret);

    sql = R"sql(
        create table if not exists repos (
                id integer primary key autoincrement,
                path text not null,
                project_id integer
          );)sql";

    ret = sqlite3_exec(dbConnection, sql.c_str(), nullptr, 0, &error);
    util::database::checkError(ret);
}

void inline util::database::checkError(int errorCode) {
    if (errorCode == SQLITE_OK) {
        return;
    }

    std::string errorMessage = sqlite3_errstr(errorCode);

    boost::log::sources::severity_logger<Severity> logger;
    BOOST_LOG_SEV(logger, Error) << errorMessage;
}
