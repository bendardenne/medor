//
// Created by bdardenn on 4/22/20.
//
#pragma once

#include <string>

#include <sqlite3.h>

namespace medor::storage {
class VcsStore {
  public:
    explicit VcsStore(sqlite3* dbConnection);

    void addRepo(const std::string& repo, const model::Project& project);

    std::optional<int> getProjectFor(const std::string& repo);

  private:
    sqlite3* _db;
};

} // namespace medor::storage
