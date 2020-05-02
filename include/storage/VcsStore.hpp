//
// Created by bdardenn on 4/22/20.
//
#pragma once

#include <sqlite3.h>
#include <string>

#include "model/Activity.hpp"

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
     * @param repo A path to a repository.
     * @param project  The project from which to remove the repo.
     */
    void removeRepo(const std::string& repo, const model::Project& project);

    /**
     * @param repo A repo path.
     * @return  The project linked to this repo, if any.
     */
    std::optional<int> getReposFor(const std::string& repo);

    /**
     * @param projectId A project ID
     * @return A vector of all the known repos for this project.
     */
    std::vector<std::string> getReposFor(int projectId);

  private:
    sqlite3* _db;
};

} // namespace medor::storage
