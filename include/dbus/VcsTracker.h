//
// Created by bdardenn on 4/22/20.
//

#pragma once

#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>
#include <sdbus-c++/sdbus-c++.h>
#include <sqlite3.h>
#include <string>

#include "storage/VcsStore.h"
#include "util/logging.h"

using namespace medor;
namespace logsrc = boost::log::sources;

namespace medor::dbus {

class VcsTracker {

  public:
    VcsTracker(sdbus::IConnection& dbusConnection, storage::VcsStore dbConnection);

  private:
    void addRepo(std::string repo, std::string project);

    void activityOnRepo(std::string repo);

    logsrc::severity_logger<Severity> _logger;
    storage::VcsStore _vcsStore;
    std::unique_ptr<sdbus::IObject> _dbusObject{};
};
} // namespace medor::dbus
