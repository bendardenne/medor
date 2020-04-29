//
// Created by bdardenn on 4/25/20.
//

#include "storage/ProjectStore.h"
#include "util/database.h"

using namespace medor::storage;
using namespace medor;

ProjectStore::ProjectStore(sqlite3* dbConnection) : _db(dbConnection) { util::database::createTables(dbConnection); }

int ProjectStore::getIdForProject(const std::string& project) {
    sqlite3_stmt* getProjectId;
    sqlite3_prepare_v2(_db, "select id from projects where name like ?", -1, &getProjectId, 0);
    sqlite3_bind_text(getProjectId, 1, project.c_str(), project.length(), SQLITE_STATIC);

    if (sqlite3_step(getProjectId) == SQLITE_ROW) { // While query has result-rows.
        int result = sqlite3_column_int(getProjectId, 0);
        sqlite3_finalize(getProjectId);
        return result;
    }

    sqlite3_stmt* newProject;
    sqlite3_prepare_v2(_db, "insert into projects (name) values (?)", -1, &newProject, 0);
    sqlite3_bind_text(newProject, 1, project.c_str(), project.length(), SQLITE_STATIC);
    sqlite3_step(newProject);
    sqlite3_step(getProjectId);
    int id = sqlite3_column_int(getProjectId, 0);
    sqlite3_finalize(newProject);

    BOOST_LOG_SEV(_logger, Info) << "Created new ID for project " << project << " (" << id << ")";

    return id;
}
std::optional<model::Project> ProjectStore::getProjectForId(const int id) {
    sqlite3_stmt* getProject;
    sqlite3_prepare_v2(_db, "select id, name from projects where id = ?", -1, &getProject, 0);
    sqlite3_bind_int(getProject, 1, id);

    std::optional<model::Project> ret;
    if (sqlite3_step(getProject) == SQLITE_ROW) { // While query has result-rows.
        std::string name(reinterpret_cast<const char*>(sqlite3_column_text(getProject, 1)));
        model::Project p = {.id = id, .name = std::string(name)};
        ret.emplace(p);
        return ret;
    }

    sqlite3_finalize(getProject);
    return ret;
}
