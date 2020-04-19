//
// Created by bdardenn on 4/15/20.
//

#ifndef MEDOR_DB_H
#define MEDOR_DB_H

#include <string>
#include <sqlite3.h>
#include <model/Activity.h>

namespace medor::storage {
    class DB {
    public:
        explicit DB(const std::string& database_file, int mode);

        ~DB();

        void add(const medor::model::Activity& activity);

        std::vector<std::string> getProjects();

        std::vector<medor::model::Activity> getActivities(std::string project, pt::time_period period);

        std::vector<medor::model::Activity> getActivities(pt::time_period period);

    private:
        static void inline checkError(int errorCode);

        void setupDb();

        sqlite3 *db;
    };
}


#endif //MEDOR_DB_H
