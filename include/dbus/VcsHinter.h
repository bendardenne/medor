//
// Created by bdardenn on 4/22/20.
//

#pragma once

#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>
#include <sdbus-c++/sdbus-c++.h>
#include <sqlite3.h>
#include <string>

#include "dbus/Tracker.h"
#include "storage/VcsStore.h"
#include "util/logging.h"

using namespace medor;
namespace logsrc = boost::log::sources;

namespace medor::dbus {

class VcsHinter {

  public:
    VcsHinter(sdbus::IConnection& dbusConnection,
              std::shared_ptr<Tracker> tracker,
              std::shared_ptr<storage::VcsStore> vcsStore,
              std::shared_ptr<storage::ProjectStore> projectStore);

  private:
    void addRepo(const std::string& project, const std::string& repo);

    void activityOnRepo(const std::string& repo);

    logsrc::severity_logger<Severity> _logger;
    std::shared_ptr<Tracker> _tracker;
    std::shared_ptr<storage::VcsStore> _vcsStore;
    std::shared_ptr<storage::ProjectStore> _projectStore;
    std::unique_ptr<sdbus::IObject> _dbusObject{};
};
} // namespace medor::dbus
