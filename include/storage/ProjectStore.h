//
// Created by bdardenn on 4/25/20.
//

#pragma once

#include <optional>
#include <sqlite3.h>
#include <string>

#include "model/Activity.h"

namespace medor::storage {

class ProjectStore {
  public:
    explicit ProjectStore(sqlite3* dbConnection);

    int getIdForProject(const std::string& basicString);

    std::optional<model::Project> getProjectForId(const int id);

  private:
    sqlite3* _db;
};

} // namespace medor::storage
