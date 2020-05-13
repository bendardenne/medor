//
// Created by bdardenn on 4/22/20.
//

#include <string>

#include "util/database.hpp"

using namespace medor;

void util::database::createTables(SQLite::Database& db) {
    std::string sql = R"sql(
        create table if not exists activities (
            id integer primary key autoincrement,
            start text not null,
            end text not null,
            comment text not null,
            project_id integer not null
        );)sql";

    db.exec(sql);

    sql = R"sql(
        create table if not exists projects (
            id integer primary key autoincrement,
            name text not null
        );)sql";

    db.exec(sql);

    sql = R"sql(
        create table if not exists repos (
            id integer primary key autoincrement,
            path text not null unique,
            project_id integer
        );)sql";

    db.exec(sql);
}