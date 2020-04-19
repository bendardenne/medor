//
// Created by bdardenn on 4/15/20.
//

#include <iostream>

#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "model/Activity.h"
#include "storage/DB.h"

using namespace medor::storage;
namespace pt = boost::posix_time;
namespace fs = boost::filesystem;

DB::DB(const std::string &database_file) {
    fs::path path(database_file);
    fs::create_directories(path.parent_path());

    int ret = sqlite3_open(database_file.c_str(), &db);
    checkError(ret);

    setupDb();
    sqlite3_prepare_v2(db, "select id from projects where name like ?", -1, &_getProjectId, 0);
    sqlite3_prepare_v2(db, "insert into projects (name) values (?)", -1, &_newProject, 0);
    sqlite3_prepare_v2(db, "insert into activities (start, end, project_id) values (?,?,?)", -1, &_newActivity, 0);
    sqlite3_prepare_v2(db, "select name from activities join projects on activities.project_id = projects.id" \
            " group by projects.id order by start desc limit ?", -1, &_getProjects, 0);

    sqlite3_prepare_v2(db, "select start,end,name from activities inner join projects on project_id = projects.id" \
            " where projects.name = ? and start between ? and ? ", -1, &_activitiesInPeriod, 0);
}

void DB::add(const model::Activity &activity) {
    std::string project = activity.getProject();

    sqlite3_bind_text(_getProjectId, 1, project.c_str(), project.length(), SQLITE_STATIC);

    std::optional<int> id;
    if (sqlite3_step(_getProjectId) == SQLITE_ROW) { // While query has result-rows.
        int result = sqlite3_column_int(_getProjectId, 0);
        id = result;
    }

    if (!id.has_value()) {
        sqlite3_bind_text(_newProject, 1, project.c_str(), project.length(), SQLITE_STATIC);
        sqlite3_step(_newProject);
        sqlite3_step(_getProjectId);
        id = sqlite3_column_int(_getProjectId, 0);
        sqlite3_reset(_newProject);
    }

    assert(id.has_value());

    const std::string startString = pt::to_iso_extended_string(activity.getStart());
    const std::string endString = pt::to_iso_extended_string(activity.getEnd());

    sqlite3_bind_text(_newActivity, 1, startString.c_str(), startString.length(), SQLITE_STATIC);
    sqlite3_bind_text(_newActivity, 2, endString.c_str(), endString.length(), SQLITE_STATIC);
    sqlite3_bind_int(_newActivity, 3, id.value());
    sqlite3_step(_newActivity);

    sqlite3_reset(_newActivity);
    sqlite3_reset(_getProjectId);
}

std::vector<std::string> DB::getProjects() const {
    std::vector<std::string> projects;

    sqlite3_bind_int(_getProjects, 1, 50);
    while (sqlite3_step(_getProjects) == SQLITE_ROW) {
        const char *name = reinterpret_cast<const char *> (sqlite3_column_text(_getProjects, 0));
        projects.emplace_back(name);
    }

    sqlite3_reset(_getProjects);
    return projects;
}

std::vector<medor::model::Activity> DB::getActivities(std::string project, pt::time_period period) const {
    std::vector<medor::model::Activity> ret;

    std::string period_start = pt::to_iso_extended_string(period.begin());
    std::string period_end = pt::to_iso_extended_string(period.end());

    sqlite3_bind_text(_activitiesInPeriod, 1, project.c_str(), project.length(), SQLITE_STATIC);
    sqlite3_bind_text(_activitiesInPeriod, 2, period_start.c_str(), period_start.length(), SQLITE_STATIC);
    sqlite3_bind_text(_activitiesInPeriod, 3, period_end.c_str(), period_end.length(), SQLITE_STATIC);

    while (sqlite3_step(_activitiesInPeriod) == SQLITE_ROW) {
        const char *start = reinterpret_cast<const char *> (sqlite3_column_text(_activitiesInPeriod, 0));
        const char *end = reinterpret_cast<const char *> (sqlite3_column_text(_activitiesInPeriod, 1));
        const char *name = reinterpret_cast<const char *> (sqlite3_column_text(_activitiesInPeriod, 2));

        ret.emplace_back(medor::model::Activity(name,
                                                pt::from_iso_extended_string(start),
                                                pt::from_iso_extended_string(end)));
    }

    sqlite3_reset(_activitiesInPeriod);
    return ret;
}

DB::~DB() {
    sqlite3_finalize(_getProjectId);
    sqlite3_finalize(_newProject);
    sqlite3_finalize(_newActivity);
    sqlite3_finalize(_getProjects);
    sqlite3_finalize(_activitiesInPeriod);

    int ret = sqlite3_close(db);
    checkError(ret);
}

void inline DB::checkError(int errorCode) {
    if (errorCode == SQLITE_OK) {
        return;
    }

    std::string errorMessage = sqlite3_errstr(errorCode);
    std::cerr << errorMessage << std::endl;
}

void DB::setupDb() {
    char *error = 0;

    std::string sql = "create table if not exists activities ( "\
                    "id integer primary key autoincrement," \
                    "start text not null," \
                    "end text not null," \
                    "comment text not null," \
                    "project_id integer not null" \
                ");";

    int ret = sqlite3_exec(db, sql.c_str(), nullptr, 0, &error);
    checkError(ret);

    sql = "create table if not exists projects ( "\
                "id integer primary key autoincrement," \
                "name text not null" \
          ");";
    ret = sqlite3_exec(db, sql.c_str(), nullptr, 0, &error);
    checkError(ret);
}
