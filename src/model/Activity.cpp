//
// Created by bdardenn on 4/15/20.
//

#include <boost/date_time/posix_time/posix_time.hpp>
#include <utility>

#include "model/Activity.h"

using namespace medor::model;


Activity::Activity(std::string project, pt::ptime start_time, pt::ptime end_time) :
        _project(std::move(project)),
        _start(start_time),
        _end(end_time) {

}

Activity::Activity(std::string project, pt::ptime start_time) : _project(std::move(project)),
                                                                _start(start_time) {

}

Activity::Activity(std::string project) : Activity(std::move(project), pt::second_clock::local_time()) {

}

std::string Activity::getProject() const {
    return _project;
}

pt::ptime Activity::getStart() const {
    return _start;
}

pt::ptime Activity::getEnd() const {
    return _end;
}

void Activity::setEnd(const pt::ptime &end) {
    _end = end;
}

std::ostream &medor::model::operator<<(std::ostream &out, const Activity &activity) {
    out << activity.getProject() << "\t\t[" << activity.getStart() << " - " << activity.getEnd() << "]";
    return out;
}
