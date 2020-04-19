//
// Created by bdardenn on 4/14/20.
//

#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <libnotify/notify.h>


#include "dbus/Tracker.h"
#include "dbus/Constants.h"
#include "util/time.h"

using namespace medor;

dbus::Tracker::Tracker(sdbus::IConnection &connection, const std::string &database_file) :
        _database(database_file),
        _dbus_object(sdbus::createObject(connection, D_TRACKER_OBJECT)) {
    namespace ph = std::placeholders;

    notify_init("medor");

    std::function<void(std::string)> start = std::bind(&Tracker::start, this, ph::_1);
    std::function<void()> stop = std::bind(&Tracker::stop, this);
    std::function<void()> resume = std::bind(&Tracker::resume, this);
    std::function<std::map<std::string, sdbus::Variant>()> status = std::bind(&Tracker::status, this);

    _dbus_object->registerMethod("start")
            .onInterface(D_TRACKER_INTERFACE)
            .implementedAs(start);

    _dbus_object->registerMethod("stop")
            .onInterface(D_TRACKER_INTERFACE)
            .implementedAs(stop);

    _dbus_object->registerMethod("resume")
            .onInterface(D_TRACKER_INTERFACE)
            .implementedAs(resume);

    _dbus_object->registerSignal("started")
            .onInterface(D_TRACKER_INTERFACE)
            .withParameters({"project"});

    _dbus_object->registerSignal("stopped")
            .onInterface(D_TRACKER_INTERFACE)
            .withParameters({"project"});

    _dbus_object->registerProperty("status")
            .onInterface(D_TRACKER_INTERFACE)
            .withGetter(status);

    _dbus_object->finishRegistration();
}

void dbus::Tracker::start(std::string project) {
    //Stop _current project, if any.
    if (this->_current.has_value()) {
        stop();
    }

    this->_current = model::Activity(std::move(project));
    this->started();
}

void dbus::Tracker::stop() {
    if (this->_current) {
        this->_current->setEnd(pt::second_clock::local_time());
        _database.add(this->_current.value());

        this->stopped();
    }

    this->_current.reset();
}

void dbus::Tracker::resume() {
    if (this->_current.has_value()) {
        return;  // do nothing
    }

    std::string lastProject = this->_database.getProjects()[0];
    this->start(lastProject);
}


void dbus::Tracker::started() const {
    model::Activity activity = this->_current.value();

    _dbus_object->emitSignal("started")
            .onInterface(D_TRACKER_INTERFACE)
            .withArguments(activity.getProject());
}

void dbus::Tracker::stopped() const {
    model::Activity activity = this->_current.value();

    _dbus_object->emitSignal("stopped")
            .onInterface(D_TRACKER_INTERFACE)
            .withArguments(activity.getProject());

    pt::time_period period(activity.getStart(), activity.getEnd());
    pt::time_duration duration = period.length();

    std::vector<model::Activity> activities = _database.getActivities(activity.getProject(),
                                                                      util::time::week_from_now(0));

    pt::time_duration thisWeek = aggregateTimes(activities);

    NotifyNotification *n = notify_notification_new("Activity stopped",
                                                    ("Stopped <b>" + activity.getProject() + "</b>" \
                                                     " after <b>" + util::time::format_duration(duration, false) + "</b>.<br/>" \
                                                     "This week: <b>" + util::time::format_duration(thisWeek, false) + "</b>.").c_str(),
                                                    NOTIFY_ICON);

    notify_notification_set_timeout(n, 5000); // 10 seconds
    notify_notification_show(n, 0);
}

std::map<std::string, sdbus::Variant> dbus::Tracker::status() const {
    std::map<std::string, sdbus::Variant> output;

    if (_current.has_value()) {
        model::Activity activity = _current.value();

        std::vector<model::Activity> activities = _database.getActivities(activity.getProject(),
                                                                          util::time::week_from_now(0));

        pt::time_duration thisWeek = aggregateTimes(activities);

        // Add current activity to this week.
        pt::time_duration currentDuration = pt::time_period(activity.getStart(),
                                                            pt::second_clock::local_time()).length();

        thisWeek += currentDuration;

        output["project"] = activity.getProject();
        output["start"] = pt::to_iso_string(activity.getStart());
        output["weekly"] = thisWeek.total_seconds();
    }

    return output;
}


pt::time_duration dbus::Tracker::aggregateTimes(const std::vector<model::Activity> &activities) {
    pt::time_duration total;

    for (const auto &activity: activities) {
        pt::time_period period(activity.getStart(), activity.getEnd());
        total += period.length();
    }

    return total;
}

dbus::Tracker::~Tracker() {
    if (_current.has_value()) {
        stop();
    }
}
