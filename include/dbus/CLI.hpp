//
// Created by bdardenn on 4/14/20.
//
#pragma once

#include <boost/date_time/posix_time/time_period.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>

#include "model/Activity.hpp"
#include "storage/ActivityStore.hpp"
#include "storage/VcsStore.hpp"
#include "util/logging.hpp"

namespace medor::dbus {
namespace pt = boost::posix_time;
namespace logsrc = boost::log::sources;

class CLI {
  public:
    explicit CLI(storage::ActivityStore activityStore, storage::VcsStore vcsStore, sdbus::IConnection& dbusConnection);

    void start(const std::string& activity);

    void stop();

    void resume();

    void status(std::string format);

    void projects(unsigned int limit);

    void report(pt::time_period period);

    void setQuiet(bool quiet);

    bool isQuiet();

    void addRepo(const std::string& project, const std::string& path);

    void removeRepo(const std::string& project, const std::string& path);

    void activityOnRepo(const std::string& path);

  private:
    void reportRepoActivity(std::vector<model::Activity> activities);

    logsrc::severity_logger<Severity> _logger;
    medor::storage::ActivityStore _activityStore;
    medor::storage::VcsStore _vcsStore;
    std::unique_ptr<sdbus::IProxy> _trackerProxy;
    std::unique_ptr<sdbus::IProxy> _vcsHinterProxy;
};
} // namespace medor::dbus
