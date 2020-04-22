//
// Created by bdardenn on 4/14/20.
//

#ifndef MEDOR_TRACKER_H
#define MEDOR_TRACKER_H

#include <optional>
#include <sdbus-c++/sdbus-c++.h>

#include "model/Activity.h"
#include "storage/ActivitiesDb.h"

namespace medor::dbus {

class Tracker {
  public:
    Tracker(sdbus::IConnection& connection, sqlite3* database_file);

    ~Tracker();

    /**
     * @return The current tracking status. For now, just a string with the active
     * project.
     */
    std::map<std::string, sdbus::Variant> status();

  private:
    /**
     * Starts tracking on a new project. Any current project is stopped.
     * @param project The new project.
     */
    void start(std::string project);

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

    // Signals
    void started();

    void stopped();

    storage::ActivitiesDb _database;
    std::unique_ptr<sdbus::IObject> _dbus_object{};
    std::optional<model::Activity> _current;
    bool _quiet = false;
};
} // namespace medor::dbus

#endif // MEDOR_TRACKER_H
