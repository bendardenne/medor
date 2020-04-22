//
// Created by bdardenn on 4/15/20.
//

#ifndef MEDOR_ACTIVITIESDB_H
#define MEDOR_ACTIVITIESDB_H

#include <string>
#include <sqlite3.h>
#include <model/Activity.h>

namespace medor::storage {
    class ActivitiesDb {
    public:
        explicit ActivitiesDb(sqlite3* db_connection);

        void add(const medor::model::Activity& activity);

        std::vector<std::string> getProjects();

        std::vector<medor::model::Activity> getActivities(std::string project, pt::time_period period);

        std::vector<medor::model::Activity> getActivities(pt::time_period period);

    private:
        static void inline checkError(int errorCode);

        sqlite3 *_db;
    };
}


#endif //MEDOR_ACTIVITIESDB_H
