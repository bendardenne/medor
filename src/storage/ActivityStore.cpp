//
// Created by bdardenn on 4/15/20.
//

#include <iostream>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/format.hpp>

#include "model/Activity.h"
#include "storage/ActivityStore.h"
#include "util/database.h"

using namespace medor;
namespace pt = boost::posix_time;

storage::ActivityStore::ActivityStore(sqlite3* dbConnection) : _db(dbConnection) {
    util::database::createTables(dbConnection);
}

void storage::ActivityStore::add(const model::Activity& activity) {
    std::string project = activity.getProject();

    sqlite3_stmt* getProjectId;
    sqlite3_prepare_v2(_db, "select id from projects where name like ?", -1, &getProjectId, 0);
    sqlite3_bind_text(getProjectId, 1, project.c_str(), project.length(), SQLITE_STATIC);

    std::optional<int> id;
    if (sqlite3_step(getProjectId) == SQLITE_ROW) { // While query has result-rows.
        int result = sqlite3_column_int(getProjectId, 0);
        id = result;
    }

    if (!id.has_value()) {
        sqlite3_stmt* newProject;
        sqlite3_prepare_v2(_db, "insert into projects (name) values (?)", -1, &newProject, 0);
        sqlite3_bind_text(newProject, 1, project.c_str(), project.length(), SQLITE_STATIC);
        sqlite3_step(newProject);
        sqlite3_step(getProjectId);
        id = sqlite3_column_int(getProjectId, 0);
        sqlite3_finalize(newProject);
    }
    sqlite3_finalize(getProjectId);

    assert(id.has_value());

    const std::string startString = pt::to_iso_extended_string(activity.getStart());
    const std::string endString = pt::to_iso_extended_string(activity.getEnd());

    sqlite3_stmt* newActivity;
    sqlite3_prepare_v2(_db, "insert into activities (start, end, project_id) values (?,?,?)", -1, &newActivity, 0);
    sqlite3_bind_text(newActivity, 1, startString.c_str(), startString.length(), SQLITE_STATIC);
    sqlite3_bind_text(newActivity, 2, endString.c_str(), endString.length(), SQLITE_STATIC);
    sqlite3_bind_int(newActivity, 3, id.value());
    sqlite3_step(newActivity);
    sqlite3_finalize(newActivity);
}

std::vector<std::string> storage::ActivityStore::getProjects() {
    std::vector<std::string> projects;

    sqlite3_stmt* getProjects;
    sqlite3_prepare_v2(_db,
                       "select name from activities join projects on "
                       "activities.project_id = projects.id"
                       " group by projects.id order by start desc limit ?",
                       -1, &getProjects, 0);
    sqlite3_bind_int(getProjects, 1, 50);
    while (sqlite3_step(getProjects) == SQLITE_ROW) {
        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(getProjects, 0));
        projects.emplace_back(name);
    }

    sqlite3_finalize(getProjects);
    return projects;
}

std::vector<medor::model::Activity> storage::ActivityStore::getActivities(std::string project, pt::time_period period) {
    std::vector<medor::model::Activity> ret;

    std::string periodStart = pt::to_iso_extended_string(period.begin());
    std::string periodEnd = pt::to_iso_extended_string(period.end());

    sqlite3_stmt* projectInPeriod;
    sqlite3_prepare_v2(_db,
                       "select start,end,name from activities inner join "
                       "projects on project_id = projects.id"
                       " where projects.name = ? and start between ? and ? ",
                       -1, &projectInPeriod, 0);
    sqlite3_bind_text(projectInPeriod, 1, project.c_str(), project.length(), SQLITE_STATIC);
    sqlite3_bind_text(projectInPeriod, 2, periodStart.c_str(), periodStart.length(), SQLITE_STATIC);
    sqlite3_bind_text(projectInPeriod, 3, periodEnd.c_str(), periodEnd.length(), SQLITE_STATIC);

    while (sqlite3_step(projectInPeriod) == SQLITE_ROW) {
        const char* start = reinterpret_cast<const char*>(sqlite3_column_text(projectInPeriod, 0));
        const char* end = reinterpret_cast<const char*>(sqlite3_column_text(projectInPeriod, 1));
        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(projectInPeriod, 2));

        ret.emplace_back(
            medor::model::Activity(name, pt::from_iso_extended_string(start), pt::from_iso_extended_string(end)));
    }

    sqlite3_finalize(projectInPeriod);
    return ret;
}

std::vector<medor::model::Activity> storage::ActivityStore::getActivities(pt::time_period period) {
    std::vector<medor::model::Activity> ret;

    std::string periodStart = pt::to_iso_extended_string(period.begin());
    std::string periodEnd = pt::to_iso_extended_string(period.end());

    sqlite3_stmt* activitiesInPeriod;
    sqlite3_prepare_v2(_db,
                       "select start,end,name from activities inner join "
                       "projects on project_id = projects.id"
                       " where start between ? and ? ",
                       -1, &activitiesInPeriod, 0);
    sqlite3_bind_text(activitiesInPeriod, 1, periodStart.c_str(), periodStart.length(), SQLITE_STATIC);
    sqlite3_bind_text(activitiesInPeriod, 2, periodEnd.c_str(), periodEnd.length(), SQLITE_STATIC);

    while (sqlite3_step(activitiesInPeriod) == SQLITE_ROW) {
        const char* start = reinterpret_cast<const char*>(sqlite3_column_text(activitiesInPeriod, 0));
        const char* end = reinterpret_cast<const char*>(sqlite3_column_text(activitiesInPeriod, 1));
        const char* name = reinterpret_cast<const char*>(sqlite3_column_text(activitiesInPeriod, 2));

        ret.emplace_back(
            medor::model::Activity(name, pt::from_iso_extended_string(start), pt::from_iso_extended_string(end)));
    }

    sqlite3_finalize(activitiesInPeriod);
    return ret;
}
