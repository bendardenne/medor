//
// Created by bdardenn on 4/14/20.
//
#pragma once

#include <string>
#include <boost/date_time/posix_time/ptime.hpp>

namespace pt = boost::posix_time;

namespace medor::model {

/**
 * A project.
 */
struct Project {
    int id;
    std::string name;
};

/**
 * An activity. It belongs to a project, and has a start time, and possibly a end time.
 */
class Activity {
  public:
    explicit Activity(Project project);

    Activity(Project project, pt::ptime startTime);

    Activity(Project project, pt::ptime startTime, pt::ptime endTime);

    Project getProject() const;

    pt::ptime getStart() const;

    pt::ptime getEnd() const;

    void setEnd(const pt::ptime& endTime);

    friend std::ostream& operator<<(std::ostream& out, const Activity& activity);

  private:
    Project _project;
    pt::ptime _start;
    pt::ptime _end;
};
} // namespace medor::model
