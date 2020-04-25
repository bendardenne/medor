//
// Created by bdardenn on 4/22/20.
//

#include <boost/optional.hpp>
#include <model/Activity.h>
#include <string>

#include "storage/VcsStore.h"
#include "util/database.h"

using namespace medor;

storage::VcsStore::VcsStore(sqlite3* dbConnection) : _db(dbConnection) { util::database::createTables(dbConnection); }

void storage::VcsStore::addRepo(const std::string& repo, const model::Project& project) {
    sqlite3_stmt* newRepo;

    sqlite3_prepare_v2(_db, "insert into repos (path, project_id) values (?,?)", -1, &newRepo, 0);
    sqlite3_bind_text(newRepo, 1, repo.c_str(), repo.length(), SQLITE_STATIC);
    sqlite3_bind_int(newRepo, 2, project.id);
    sqlite3_step(newRepo);
    sqlite3_finalize(newRepo);
}

std::optional<int> storage::VcsStore::getProjectFor(const std::string& repo) {
    sqlite3_stmt* getProjectFor;
    sqlite3_prepare_v2(_db, "select id, path, project_id from repos where path like ?", -1, &getProjectFor, 0);
    sqlite3_bind_text(getProjectFor, 1, repo.c_str(), repo.length(), SQLITE_STATIC);

    std::optional<int> ret;
    if (sqlite3_step(getProjectFor) == SQLITE_ROW) {
        ret.emplace(sqlite3_column_int(getProjectFor, 2));
    }
    sqlite3_finalize(getProjectFor);
    return ret;
}
