//
// Created by bdardenn on 4/15/20.
//

#ifndef MEDOR_ACTIVITYSTORE_H
#define MEDOR_ACTIVITYSTORE_H

#include <sqlite3.h>
#include <string>

#include "model/Activity.h"

namespace medor::storage {
class ActivityStore {
  public:
    explicit ActivityStore(sqlite3* db_connection);

    void add(const medor::model::Activity& activity);

    std::vector<std::string> getProjects();

    std::vector<medor::model::Activity> getActivities(std::string project, pt::time_period period);

    std::vector<medor::model::Activity> getActivities(pt::time_period period);

  private:
    static void inline checkError(int errorCode);

    sqlite3* _db;
};
} // namespace medor::storage

#endif // MEDOR_ACTIVITYSTORE_H
