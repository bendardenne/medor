//
// Created by bdardenn on 4/15/20.
//

#include <iostream>

#include <boost/format.hpp>

#include "model/Activity.hpp"
#include "storage/ActivityStore.hpp"

using namespace medor;
namespace pt = boost::posix_time;

storage::ActivityStore::ActivityStore(Database db) : _db(db) {}

void storage::ActivityStore::add(const model::Activity& activity) {
    model::Project project = activity.getProject();

    const std::string startString = pt::to_iso_extended_string(activity.getStart());
    const std::string endString = pt::to_iso_extended_string(activity.getEnd());

    SQLite::Database handle = _db.handle();
    SQLite::Statement query(handle, "insert into activities (start, end, project_id) values (?,?,?)");
    query.bind(1, startString);
    query.bind(2, endString);
    query.bind(3, project.id);
    query.exec();
}

std::vector<std::string> storage::ActivityStore::getRecentProjects(unsigned int limit) {
    std::vector<std::string> projects;
    SQLite::Database handle = _db.handle();
    SQLite::Statement query(handle,
                            R"sql(select max(activities.start), projects.name from activities join projects on
                       activities.project_id = projects.id
                       group by projects.id order by start desc limit ? )sql");

    query.bind(1, limit);
    while (query.executeStep()) {
        projects.emplace_back(query.getColumn(1));
    }

    return projects;
}

std::vector<medor::model::Activity> storage::ActivityStore::getActivities(const model::Project& project,
                                                                          pt::time_period period) {
    std::vector<medor::model::Activity> ret;
    std::string periodStart = pt::to_iso_extended_string(period.begin());
    std::string periodEnd = pt::to_iso_extended_string(period.end());

    SQLite::Database handle = _db.handle();
    SQLite::Statement query(handle,
                            R"sql(select start,end,name,project_id from activities inner join
                       projects on project_id = projects.id
                       where projects.id = ? and start between ? and ? )sql");

    query.bind(1, project.id);
    query.bind(2, periodStart);
    query.bind(3, periodEnd);

    while (query.executeStep()) {
        std::string start = query.getColumn(0);
        std::string end = query.getColumn(1);
        std::string name = query.getColumn(2);
        int projectId = query.getColumn(3);

        ret.emplace_back(model::Activity(
            {.id = projectId, .name = name}, pt::from_iso_extended_string(start), pt::from_iso_extended_string(end)));
    }

    return ret;
}

std::vector<medor::model::Activity> storage::ActivityStore::getActivities(pt::time_period period) {
    std::vector<medor::model::Activity> ret;

    std::string periodStart = pt::to_iso_extended_string(period.begin());
    std::string periodEnd = pt::to_iso_extended_string(period.end());

    SQLite::Database handle = _db.handle();
    SQLite::Statement query(handle,
                            R"sql(select start,end,name,project_id from activities inner join
                       projects on project_id = projects.id
                       where start between ? and ? )sql");

    query.bind(1, periodStart);
    query.bind(2, periodEnd);

    while (query.executeStep()) {
        const char* start = query.getColumn(0);
        const char* end = query.getColumn(1);
        const char* name = query.getColumn(2);
        int projectId = query.getColumn(3);

        ret.emplace_back(model::Activity(
            {.id = projectId, .name = name}, pt::from_iso_extended_string(start), pt::from_iso_extended_string(end)));
    }

    return ret;
}
