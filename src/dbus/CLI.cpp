//
// Created by bdardenn on 4/14/20.
//

#include <sdbus-c++/IProxy.h>
#include <iostream>
#include <boost/algorithm/string/replace.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "dbus/CLI.h"
#include "dbus/Constants.h"
#include "util/time.h"

using namespace medor;

namespace greg = boost::gregorian;

dbus::CLI::CLI(std::string database_file) : _database(database_file, SQLITE_OPEN_READONLY),
                                            _trackerProxy(sdbus::createProxy(D_SERVICE_NAME, D_TRACKER_OBJECT)) {
}

void dbus::CLI::start(const std::string &activity) {
    _trackerProxy->callMethod("start")
            .onInterface(D_TRACKER_INTERFACE)
            .withArguments(activity);
}

void dbus::CLI::stop() {
    _trackerProxy->callMethod("stop")
            .onInterface(D_TRACKER_INTERFACE);
}

void dbus::CLI::resume() {
    _trackerProxy->callMethod("resume")
            .onInterface(D_TRACKER_INTERFACE);
}

void dbus::CLI::status(std::string format) {
    std::map<std::string, sdbus::Variant> status =
            _trackerProxy->getProperty("status").onInterface(D_TRACKER_INTERFACE);

    if (status.count("project") > 0) {
        pt::ptime start = pt::from_iso_string(status["start"].get<std::string>());
        pt::ptime now = pt::second_clock::local_time();
        pt::time_period period(start, now);

        pt::time_duration weekly = pt::seconds(status["weekly"].get<long>());

        boost::algorithm::replace_all(format, "%p", status["project"].get<std::string>());
        boost::algorithm::replace_all(format, "%d", util::time::format_duration(period.length(), true));
        boost::algorithm::replace_all(format, "%w", util::time::format_duration(weekly, true));

        std::cout << format << std::endl;
    } else {
        std::cout << "No project started" << std::endl;
    }
}

void dbus::CLI::projects() {
    // TODO this does not necessarily include the current project, or it may not be the first of the list.
    //  Maybe not a problem?
    std::vector<std::string> projects = _database.getProjects();

    for (const auto &project : projects) {
        std::cout << project << std::endl;
    }
}

void dbus::CLI::report(pt::time_period period) {
    //FIXME doesn't include current activity
    pt::time_period this_week = util::time::week_from_now(0);
    std::vector<model::Activity> all_activities = _database.getActivities(this_week);

    std::map<greg::date, std::vector<model::Activity>> by_day;

    for (const auto &activity : all_activities) {
        by_day[activity.getStart().date()].emplace_back(activity);
    }

    for (const auto &for_day : by_day) {
        std::cout << for_day.first.day_of_week().as_long_string() << std::endl;
        std::map<std::string, std::vector<model::Activity>> by_project;

        for (const auto &activity: for_day.second) {
            by_project[activity.getProject()].emplace_back(activity);
        }

        for (const auto &project: by_project) {
            const std::string &spent_on_project = util::time::format_duration(
                    util::time::aggregateTimes(project.second), false);
            std::cout << "\t\t" << project.first << ": " << spent_on_project << std::endl;
        }
    }
}