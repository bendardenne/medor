//
// Created by bdardenn on 4/14/20.
//

#include <boost/algorithm/string/replace.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <iostream>
#include <sdbus-c++/IProxy.h>

#include "dbus/CLI.h"
#include "dbus/Constants.h"
#include "util/time.h"

using namespace medor;

namespace greg = boost::gregorian;

dbus::CLI::CLI(storage::ActivityStore activityStore)
    : _activityStore(activityStore), _trackerProxy(sdbus::createProxy(D_SERVICE_NAME, D_TRACKER_OBJECT)) {}

void dbus::CLI::start(const std::string& activity) {
    _trackerProxy->callMethod("start").onInterface(D_TRACKER_INTERFACE).withArguments(activity);
}

void dbus::CLI::stop() { _trackerProxy->callMethod("stop").onInterface(D_TRACKER_INTERFACE); }

void dbus::CLI::resume() { _trackerProxy->callMethod("resume").onInterface(D_TRACKER_INTERFACE); }

void dbus::CLI::status(std::string format) {
    std::map<std::string, sdbus::Variant> status =
        _trackerProxy->getProperty("status").onInterface(D_TRACKER_INTERFACE);

    if (status.count("project") > 0) {
        pt::ptime start = pt::from_iso_string(status["start"].get<std::string>());
        pt::ptime now = pt::second_clock::local_time();
        pt::time_period period(start, now);

        pt::time_duration weekly = pt::seconds(status["weekly"].get<long>());

        boost::algorithm::replace_all(format, "%i", std::to_string(status["project_id"].get<int>()));
        boost::algorithm::replace_all(format, "%p", status["project"].get<std::string>());
        boost::algorithm::replace_all(format, "%d", util::time::format_duration(period.length(), true));
        boost::algorithm::replace_all(format, "%w", util::time::format_duration(weekly, true));

        std::cout << format << std::endl;
    } else {
        std::cout << "No project started" << std::endl;
    }
}

void dbus::CLI::projects() {
    // TODO this does not necessarily include the current project, or it may not
    // be the first of the list.
    //  Maybe not a problem?
    std::vector<std::string> projects = _activityStore.getProjects();

    for (const auto& project : projects) {
        std::cout << project << std::endl;
    }
}

void dbus::CLI::report(pt::time_period period) {
    pt::time_period thisWeek = util::time::week_from_now(0);
    std::vector<model::Activity> allActivities = _activityStore.getActivities(thisWeek);

    // Fetch and account for current activity, if any.
    std::map<std::string, sdbus::Variant> status =
        _trackerProxy->getProperty("status").onInterface(D_TRACKER_INTERFACE);
    if (status.count("project") > 0) {
        model::Project project = {.id = status["project_id"], .name = status["project"]};
        pt::ptime start = pt::from_iso_string(status["start"].get<std::string>());
        pt::ptime now = pt::second_clock::local_time();
        allActivities.emplace_back(model::Activity(project, start, now));
    }

    std::map<greg::date, std::vector<model::Activity>> byDay;

    for (const auto& activity : allActivities) {
        byDay[activity.getStart().date()].emplace_back(activity);
    }

    for (const auto& forDay : byDay) {
        std::cout << forDay.first.day_of_week().as_long_string() << std::endl;
        std::map<int, std::vector<model::Activity>> byProject;

        for (const auto& activity : forDay.second) {
            byProject[activity.getProject().id].emplace_back(activity);
        }

        for (const auto& project : byProject) {
            const std::string& spentOnProject =
                util::time::format_duration(util::time::aggregateTimes(project.second), false);
            std::cout << "\t\t" << project.second[0].getProject().name << ": " << spentOnProject << std::endl;
        }
    }
}

void dbus::CLI::setQuiet(bool quiet) {
    _trackerProxy->setProperty("quiet").onInterface(D_TRACKER_INTERFACE).toValue(quiet);
}
