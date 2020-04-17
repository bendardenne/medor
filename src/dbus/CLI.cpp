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

using namespace medor;
namespace pt = boost::posix_time;

dbus::CLI::CLI() {
    trackerProxy = sdbus::createProxy(D_SERVICE_NAME, D_TRACKER_OBJECT);
}

void dbus::CLI::start(const std::string &activity) {
    trackerProxy->callMethod("start")
            .onInterface(D_TRACKER_INTERFACE)
            .withArguments(activity);
}

void dbus::CLI::stop() {
    trackerProxy->callMethod("stop")
            .onInterface(D_TRACKER_INTERFACE);
}

void dbus::CLI::resume() {
    trackerProxy->callMethod("resume")
            .onInterface(D_TRACKER_INTERFACE);
}

void dbus::CLI::status(std::string format) {
    std::map<std::string, sdbus::Variant> status =
            trackerProxy->getProperty("status").onInterface(D_TRACKER_INTERFACE);

    if (status.count("project") > 0) {
        pt::ptime start = pt::from_iso_string(status["start"].get<std::string>());
        pt::ptime now = pt::second_clock::local_time();

        pt::time_period period(start, now);

        boost::algorithm::replace_all(format, "%p", status["project"].get<std::string>());
        boost::algorithm::replace_all(format, "%d", format_duration(period.length()));

        std::cout << format << std::endl;
    } else {
        std::cout << "No project started" << std::endl;
    }
}

void dbus::CLI::projects() {
    std::vector<std::string> projects =
            trackerProxy->getProperty("projects").onInterface(D_TRACKER_INTERFACE);

    for (const auto &project : projects) {
        std::cout << project << std::endl;
    }
}

std::string dbus::CLI::format_duration(pt::time_duration duration) {
    std::stringstream ret;

    if (duration.hours() >= 24) {
        ret << std::to_string((int) std::floor(duration.hours() / 24)) + "d";
    }

    if (duration.hours() > 0) {
        ret << std::to_string(duration.hours() % 24) + "h";
    }

    ret << std::to_string(duration.minutes()) + "m";
    return ret.str();
}