//
// Created by bdardenn on 4/14/20.
//

#include <boost/algorithm/string/replace.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/ptime.hpp>
#include <iostream>
#include <rang.hpp>
#include <sdbus-c++/IProxy.h>

#include "dbus/CLI.hpp"
#include "dbus/Constants.hpp"
#include "storage/VcsStore.hpp"
#include "util/time.hpp"
#include "vcs/HgClient.hpp"

using namespace medor;

namespace greg = boost::gregorian;
typedef rang::style style;

dbus::CLI::CLI(storage::ActivityStore activityStore, storage::VcsStore vcsStore, sdbus::IConnection& dbusConnection)
    : _activityStore(activityStore), _vcsStore(vcsStore),
      _trackerProxy(sdbus::createProxy(dbusConnection, D_SERVICE_NAME, D_TRACKER_OBJECT)),
      _vcsHinterProxy(sdbus::createProxy(dbusConnection, D_SERVICE_NAME, D_VCSHINTER_OBJECT)) {}

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
        boost::algorithm::replace_all(format, "%d", util::time::formatDuration(period.length(), true));
        boost::algorithm::replace_all(format, "%w", util::time::formatDuration(weekly, true));

        std::cout << format << std::endl;
    } else {
        std::cout << "No project started" << std::endl;
    }
}

void dbus::CLI::projects(unsigned int limit) {
    std::vector<std::string> projects = _activityStore.getRecentProjects(limit);

    for (const auto& project : projects) {
        std::cout << project << std::endl;
    }
}

void dbus::CLI::report(pt::time_period period) {
    std::vector<model::Activity> allActivities = _activityStore.getActivities(period);

    // Fetch and account for current activity, if any.
    std::map<std::string, sdbus::Variant> status =
        _trackerProxy->getProperty("status").onInterface(D_TRACKER_INTERFACE);
    if (status.count("project") > 0) {
        pt::ptime start = pt::from_iso_string(status["start"].get<std::string>());
        if (period.contains(start)) {
            model::Project project = {.id = status["project_id"], .name = status["project"]};
            pt::ptime now = pt::second_clock::local_time();
            allActivities.emplace_back(model::Activity(project, start, now));
        }
    }

    std::map<greg::date, std::vector<model::Activity>> byDay;

    // Group by day
    for (const auto& activity : allActivities) {
        byDay[activity.getStart().date()].emplace_back(activity);
    }

    for (const auto& forDay : byDay) {
        std::cout << style::bold << forDay.first.day_of_week().as_long_string() << style::reset << std::endl;
        std::map<int, std::vector<model::Activity>> byProject;

        // Group by project, for this day
        for (const auto& activity : forDay.second) {
            byProject[activity.getProject().id].emplace_back(activity);
        }

        // For every project, print spent time, on that day
        for (const auto& activities : byProject) {
            const model::Project& currentProject = activities.second[0].getProject();
            const std::string& spentOnProject =
                util::time::formatDuration(util::time::aggregateTimes(activities.second), false);

            std::cout << "\t" << style::underline << currentProject.name << style::reset << ": " << spentOnProject
                      << std::endl;

            // And possibly VCS activity on this project, for this day
            reportRepoActivity(activities.second);
        }

        std::cout << std::endl;
    }
}

void dbus::CLI::reportRepoActivity(std::vector<model::Activity> activities) {
    const model::Project& currentProject = activities[0].getProject();
    std::vector<std::string> repos = _vcsStore.getReposFor(currentProject.id);

    // Instead of checking each single period, we could make a single period that spans all periods of all activities.
    // This would mean a single call to log(), and would include possible commits done when the project was no active.
    // Which is desirable? Probably

    std::stringstream ss;
    ss.imbue(std::locale(std::locale::classic(), new pt::time_facet("%H:%M")));
    if (!repos.empty()) {
        for (const auto& repo : repos) {
            try {
                std::unique_ptr<vcs::IVcsClient> vcs = vcs::IVcsClient::create(repo);
                if (!vcs) {
                    BOOST_LOG_SEV(_logger, Error)
                        << "Could not open repo (" << currentProject.name << ") " << repo << ": "
                        << " not a know vcs root";
                    continue;
                }
                for (const auto& activity : activities) {
                    for (const auto& entry : vcs->log(activity.getPeriod())) {
                        ss.str("");
                        ss << entry.date;
                        std::cout << "\t\t(" << ss.str() << ") " << entry.summary << std::endl;
                    }
                }
            } catch (std::exception& e) {
                BOOST_LOG_SEV(_logger, Error)
                    << "Could not open repo (" << currentProject.name << ") " << repo << ": " << e.what();
            }
        }
    }
}

void dbus::CLI::setQuiet(bool quiet) {
    _trackerProxy->setProperty("quiet").onInterface(D_TRACKER_INTERFACE).toValue(quiet);
}

bool dbus::CLI::isQuiet() { return _trackerProxy->getProperty("quiet").onInterface(D_TRACKER_INTERFACE).get<bool>(); }

void dbus::CLI::addRepo(const std::string& project, const std::string& path) {
    _vcsHinterProxy->callMethod("addRepo").onInterface(D_VCSHINTER_INTERFACE).withArguments(project, path);
}

void dbus::CLI::removeRepo(const std::string& project, const std::string& path) {
    _vcsHinterProxy->callMethod("removeRepo").onInterface(D_VCSHINTER_INTERFACE).withArguments(project, path);
}

void dbus::CLI::activityOnRepo(const std::string& path) {
    _vcsHinterProxy->callMethod("activityOnRepo").onInterface(D_VCSHINTER_INTERFACE).withArguments(path);
}
