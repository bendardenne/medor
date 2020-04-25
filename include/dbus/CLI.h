//
// Created by bdardenn on 4/14/20.
//
#pragma once

#include <boost/date_time/posix_time/time_period.hpp>

#include "storage/ActivityStore.h"

namespace medor::dbus {
namespace pt = boost::posix_time;

class CLI {
  public:
    explicit CLI(storage::ActivityStore activityStore);

    void start(const std::string& activity);

    void stop();

    void resume();

    void status(std::string format);

    void projects();

    void report(pt::time_period period);

    void setQuiet(bool quiet);

    void addRepo(std::basic_string<char> project, std::basic_string<char> path);

  private:
    medor::storage::ActivityStore _activityStore;
    std::unique_ptr<sdbus::IProxy> _trackerProxy;
    std::unique_ptr<sdbus::IProxy> _vcsHinterProxy;
};
} // namespace medor::dbus
