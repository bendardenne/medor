//
// Created by bdardenn on 4/25/20.
//

#include "storage/ProjectStore.hpp"
#include "storage/Database.hpp"
#include "util/database.hpp"

using namespace medor::storage;
using namespace medor;

ProjectStore::ProjectStore(Database db) : _db(db) {}

int ProjectStore::getIdForProject(const std::string& project) {
    SQLite::Database handle = _db.handle();
    SQLite::Statement query(handle, "select id from projects where name like ?");

    query.bind(1, project);

    if (query.executeStep()) { // While query has result-rows.
        int result = query.getColumn(0);
        return result;
    }

    SQLite::Statement insert(handle, "insert into projects (name) values (?)");
    insert.bind(1, project);
    insert.exec();

    query.reset();
    query.executeStep();
    int id = query.getColumn(0);

    BOOST_LOG_SEV(_logger, Info) << "Created new ID for project " << project << " (" << id << ")";

    return id;
}
std::optional<model::Project> ProjectStore::getProjectForId(const int id) {
    SQLite::Database handle = _db.handle();
    SQLite::Statement query(handle, "select id, name from projects where id = ?");
    query.bind(1, id);

    std::optional<model::Project> ret;
    if (query.executeStep()) { // While query has result-rows.
        model::Project p = {.id = id, .name = query.getColumn(1)};
        ret.emplace(p);
        return ret;
    }

    return ret;
}
