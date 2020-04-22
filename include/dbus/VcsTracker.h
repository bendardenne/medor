//
// Created by bdardenn on 4/22/20.
//

#ifndef MEDOR_VCSTRACKER_H
#define MEDOR_VCSTRACKER_H

#include <sdbus-c++/sdbus-c++.h>
#include <sqlite3.h>
#include <string>

#include "storage/VcsDb.h"

using namespace medor;

namespace medor::dbus {

class VcsTracker {

  public:
    VcsTracker(sdbus::IConnection& dbus_connection, sqlite3* db_connection);

  private:
    void newRepo(std::string repo, std::string project);

    void activityOnRepo(std::string repo);

    storage::VcsDb _database;
    std::unique_ptr<sdbus::IObject> _dbus_object{};
};
} // namespace medor::dbus

#endif // MEDOR_VCSTRACKER_H
