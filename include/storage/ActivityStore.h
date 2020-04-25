//
// Created by bdardenn on 4/15/20.
//
#pragma once

#include <sqlite3.h>
#include <string>

#include "model/Activity.h"

namespace medor::storage {
class ActivityStore {
  public:
    explicit ActivityStore(sqlite3* dbConnection);

    void add(const medor::model::Activity& activity);

    std::vector<std::string> getProjects();

    std::vector<medor::model::Activity> getActivities(const model::Project& project, pt::time_period period);

    std::vector<medor::model::Activity> getActivities(pt::time_period period);

    int getIdForProject(const std::string& basicString);

  private:
    sqlite3* _db;
};
} // namespace medor::storage
