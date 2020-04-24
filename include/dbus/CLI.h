//
// Created by bdardenn on 4/14/20.
//

#ifndef MEDOR_CLI_H
#define MEDOR_CLI_H

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

  private:
    medor::storage::ActivityStore _activityStore;
    std::unique_ptr<sdbus::IProxy> _trackerProxy;
};
} // namespace medor::dbus

#endif // MEDOR_CLI_H
