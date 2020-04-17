//
// Created by bdardenn on 4/14/20.
//

#ifndef MEDOR_ACTIVITY_H
#define MEDOR_ACTIVITY_H

#include <boost/date_time/posix_time/ptime.hpp>

namespace pt = boost::posix_time;

namespace medor::model {

    class Activity {
    public:
        explicit Activity(std::string project);

        Activity(std::string project, pt::ptime start_time);

        Activity(std::string project, pt::ptime start_time, pt::ptime end_time);

        std::string getProject() const;

        pt::ptime getStart() const;

        pt::ptime getEnd() const;

        void setEnd(const pt::ptime &endTime);


    private:
        std::string _project;
        pt::ptime _start;
        pt::ptime _end;
    };
}


#endif //MEDOR_ACTIVITY_H
