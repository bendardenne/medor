//
// Created by bdardenn on 4/22/20.
//
#pragma once

#include <string>

#include <sqlite3.h>

namespace medor::storage {

/**
 * A store that keeps track of all registered VCS roots.
 */
class VcsStore {
  public:
    explicit VcsStore(sqlite3* dbConnection);

    /**
     * @param repo A path to a repository.
     * @param project  A project to link to the repository.
     */
    void addRepo(const std::string& repo, const model::Project& project);

    /**
     * @param repo A repo path.
     * @return  The project linked to this repo, if any.
     */
    std::optional<int> getProjectFor(const std::string& repo);

  private:
    sqlite3* _db;
};

} // namespace medor::storage
