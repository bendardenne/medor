//
// Created by bdardenn on 4/15/20.
//
#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>
#include <sqlite3.h>
#include <string>

#include "model/Activity.hpp"
#include "storage/Database.hpp"

namespace medor::storage {
/**
 * A store that can manage the registered activities.
 */
class ActivityStore {
  public:
    explicit ActivityStore(Database database);

    /**
     * @param activity A new activity to be registered.
     */
    void add(const medor::model::Activity& activity);

    /**
     * Get a list of the recent projects, ordered by most recent first.
     * @param limit The maximum of projects to return (Default: 50)
     * @return
     */
    std::vector<std::string> getRecentProjects(unsigned int limit = 50);

    /**
     * @param project A project.
     * @param period  A time period.
     * @return All the activities that started in the given period, for the given project.
     */
    std::vector<medor::model::Activity> getActivities(const model::Project& project, pt::time_period period);

    /**
     * @param period A time period.
     * @return  All activities that started in the given period (all projects).
     */
    std::vector<medor::model::Activity> getActivities(pt::time_period period);


  private:
    Database _db;
};
} // namespace medor::storage
