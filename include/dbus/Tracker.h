//
// Created by bdardenn on 4/14/20.
//
#pragma once

#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>
#include <optional>
#include <sdbus-c++/sdbus-c++.h>

#include "model/Activity.h"
#include "storage/ActivityStore.h"
#include "storage/ProjectStore.h"
#include "util/logging.h"

namespace logsrc = boost::log::sources;
namespace medor::dbus {

class Tracker {
  public:
    Tracker(sdbus::IConnection& connection,
            std::shared_ptr<storage::ActivityStore> activityStore,
            std::shared_ptr<storage::ProjectStore> projectStore);

    /**
     * @return The current tracking status. For now, just a string with the active
     * project.
     */
    std::map<std::string, sdbus::Variant> status();

    /**
     * Starts tracking on a new project. Any current project is stopped.
     * @param project The new project.
     */
    void start(const std::string& project);

    /**
     * Stop tracking the active project, if any.
     */
    void stop();

    /**
     * Restart the last active project. If already tracking, does nothing.
     */
    void resume();

    bool isQuiet() const;

    void setQuiet(bool quiet);

  private:
    // Signals
    void started();

    void stopped();

    logsrc::severity_logger<Severity> _logger;
    std::shared_ptr<storage::ActivityStore> _activities;
    std::shared_ptr<storage::ProjectStore> _projects;

    std::unique_ptr<sdbus::IObject> _dbusObject{};
    std::optional<model::Activity> _current;
    bool _quiet = false;
};
} // namespace medor::dbus
