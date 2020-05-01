//
// Created by bdardenn on 4/15/20.
//
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/time_clock.hpp>
#include <utility>

#include "model/Activity.h"

using namespace medor::model;
namespace pt = boost::posix_time;

Activity::Activity(Project project) : Activity(std::move(project), pt::second_clock::local_time()) {}

Activity::Activity(Project project, pt::ptime startTime, pt::ptime endTime)
    : _project(std::move(project)), _start(startTime), _end(endTime) {}

Activity::Activity(Project project, pt::ptime startTime) : _project(std::move(project)), _start(startTime) {}

Project Activity::getProject() const { return _project; }

pt::ptime Activity::getStart() const { return _start; }

pt::ptime Activity::getEnd() const { return _end; }

pt::time_period Activity::getPeriod() const { return pt::time_period(getStart(), getEnd()); }

void Activity::setEnd(const pt::ptime& end) { _end = end; }

std::ostream& medor::model::operator<<(std::ostream& out, const Activity& activity) {
    out << activity.getProject().name << "\t\t[" << activity.getStart() << " - " << activity.getEnd() << "]";
    return out;
}
