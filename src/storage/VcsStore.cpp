//
// Created by bdardenn on 4/22/20.
//

#include <boost/optional.hpp>
#include <string>

#include "model/Activity.hpp"
#include "storage/VcsStore.hpp"
#include "util/database.hpp"

using namespace medor;

storage::VcsStore::VcsStore(Database db) : _db(db) {}

bool storage::VcsStore::addRepo(const std::string& repo, const model::Project& project) {
    SQLite::Database handle = _db.handle();
    SQLite::Statement query(handle, "insert into repos (path, project_id) values (?,?)");
    query.bind(1, repo.c_str());
    query.bind(2, project.id);
    query.exec();
    return handle.getTotalChanges() != 0;
}

bool storage::VcsStore::removeRepo(const std::string& repo, const model::Project& project) {
    SQLite::Database handle = _db.handle();
    SQLite::Statement query(handle, "delete from repos where path is ? and project_id is ? ");
    query.bind(1, repo.c_str());
    query.bind(2, project.id);
    query.exec();
    return handle.getTotalChanges() != 0;
}

std::optional<int> storage::VcsStore::getReposFor(const std::string& repo) {
    SQLite::Database handle = _db.handle();
    SQLite::Statement query(handle, "select id, path, project_id from repos where path like ?");
    query.bind(1, repo);

    std::optional<int> ret;
    if (query.executeStep()) {
        ret.emplace(query.getColumn(2));
    }
    return ret;
}

std::vector<std::string> storage::VcsStore::getReposFor(int projectId) {
    SQLite::Database handle = _db.handle();
    SQLite::Statement query(handle, "select path from repos where project_id is ?");
    query.bind(1, projectId);

    std::vector<std::string> result;
    while (query.executeStep()) {
        result.emplace_back(query.getColumn(0));
    }

    return result;
}
