//
// Created by bdardenn on 4/25/20.
//

#pragma once

#include <optional>
#include <sqlite3.h>
#include <string>

#include "model/Activity.h"

namespace medor::storage {

/**
 * A store that manages the known projects.
 */
class ProjectStore {
  public:
    explicit ProjectStore(sqlite3* dbConnection);

    /**
     * @param project A project name
     * @return The ID of this project. If the project does not exist, a new one will be created.
     */
    int getIdForProject(const std::string& project);

    /**
     * @param id An identifier
     * @return The project for this ID, if any.
     */
    std::optional<model::Project> getProjectForId(const int id);

  private:
    sqlite3* _db;
};

} // namespace medor::storage
