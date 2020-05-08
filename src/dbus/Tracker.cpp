//
// Created by bdardenn on 4/14/20.
//

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <iostream>
#include <utility>

#include "dbus/Constants.hpp"
#include "dbus/Tracker.hpp"
#include "util/time.hpp"

using namespace medor;

dbus::Tracker::Tracker(sdbus::IConnection& connection,
                       std::shared_ptr<Notifier> notifier,
                       std::shared_ptr<storage::ActivityStore> activityStore,
                       std::shared_ptr<storage::ProjectStore> projectStore)
    : _notifier(std::move(notifier)), _activities(std::move(activityStore)), _projects(std::move(projectStore)),
      _dbusObject(sdbus::createObject(connection, D_TRACKER_OBJECT)) {
    namespace ph = std::placeholders;

    std::function<void(std::string)> start = std::bind(&Tracker::start, this, ph::_1);
    std::function<void()> stop = std::bind(&Tracker::stop, this);
    std::function<void()> resume = std::bind(&Tracker::resume, this);
    std::function<std::map<std::string, sdbus::Variant>()> status = std::bind(&Tracker::status, this);
    std::function<bool()> isQuiet = std::bind(&Tracker::isQuiet, this);
    std::function<void(bool)> setQuiet = std::bind(&Tracker::setQuiet, this, ph::_1);

    _dbusObject->registerMethod("start").onInterface(D_TRACKER_INTERFACE).implementedAs(start);
    _dbusObject->registerMethod("stop").onInterface(D_TRACKER_INTERFACE).implementedAs(stop);
    _dbusObject->registerMethod("resume").onInterface(D_TRACKER_INTERFACE).implementedAs(resume);

    _dbusObject->registerSignal("started").onInterface(D_TRACKER_INTERFACE).withParameters({"project"});
    _dbusObject->registerSignal("stopped").onInterface(D_TRACKER_INTERFACE).withParameters({"project"});

    _dbusObject->registerProperty("status").onInterface(D_TRACKER_INTERFACE).withGetter(status);
    _dbusObject->registerProperty("quiet").onInterface(D_TRACKER_INTERFACE).withGetter(isQuiet).withSetter(setQuiet);

    _dbusObject->finishRegistration();
}

void dbus::Tracker::start(const std::string& project) {
    // Stop _current project, if any.
    if (this->_current.has_value()) {
        stop();
    }

    this->_current = model::Activity({.id = _projects->getIdForProject(project), .name = project});

    model::Activity activity = this->_current.value();
    _dbusObject->emitSignal("started").onInterface(D_TRACKER_INTERFACE).withArguments(activity.getProject().name);
    BOOST_LOG_SEV(_logger, Info) << "Activity on " + activity.getProject().name + " started";
}

void dbus::Tracker::stop() {
    if (this->_current) {
        model::Activity activity = this->_current.value();
        activity.setEnd(pt::second_clock::local_time());

        BOOST_LOG_SEV(_logger, Info) << "Activity on " << activity.getProject().name << " stopped";

        if (this->_current->getPeriod().length().minutes() > 5) {
            _activities->add(activity);
            pt::time_duration duration = activity.getPeriod().length();
            std::vector<model::Activity> activities =
                _activities->getActivities(activity.getProject(), util::time::weekFromNow(0));

            pt::time_duration thisWeek = util::time::aggregateTimes(activities);

            // Send notification.
            std::stringstream ss;
            ss << "Stopped <b>" << activity.getProject().name << "</b> after ";
            ss << "<b>" << util::time::formatDuration(duration, false) << "</b>.<br/>";
            ss << "This week: <b>" << util::time::formatDuration(thisWeek, false) << "</b>";
            _notifier->send("Activity stopped", ss.str());
        } else {
            BOOST_LOG_SEV(_logger, Info) << "Activity on " << activity.getProject().name
                                         << " was not recorded: too short";
        }
        this->_current.reset();
        _dbusObject->emitSignal("stopped").onInterface(D_TRACKER_INTERFACE).withArguments(activity.getProject().name);
    }
}

void dbus::Tracker::resume() {
    if (this->_current.has_value()) {
        return; // do nothing
    }

    auto activities = this->_activities->getRecentProjects(1);

    if (activities.empty()) {
        BOOST_LOG_SEV(_logger, Error) << "No project to resume.";
        return;
    }

    std::string lastProject = activities[0];
    this->start(lastProject);
}

std::map<std::string, sdbus::Variant> dbus::Tracker::status() {
    std::map<std::string, sdbus::Variant> output;

    if (_current.has_value()) {
        model::Activity activity = _current.value();

        std::vector<model::Activity> activities =
            _activities->getActivities(activity.getProject(), util::time::weekFromNow(0));

        pt::time_duration thisWeek = util::time::aggregateTimes(activities);

        // Add current activity to this week.
        pt::time_duration currentDuration =
            pt::time_period(activity.getStart(), pt::second_clock::local_time()).length();

        thisWeek += currentDuration;

        output["project_id"] = activity.getProject().id;
        output["project"] = activity.getProject().name;
        output["start"] = pt::to_iso_string(activity.getStart());
        output["weekly"] = thisWeek.total_seconds();
    }

    return output;
}

bool dbus::Tracker::isQuiet() const { return _notifier->isQuiet(); }

void dbus::Tracker::setQuiet(bool quiet) { _notifier->setQuiet(quiet); }
