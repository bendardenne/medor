//
// Created by bdardenn on 4/22/20.
//

#include "dbus/VcsTracker.h"
#include "dbus/Constants.h"

using namespace medor;

dbus::VcsTracker::VcsTracker(sdbus::IConnection &dbus_connection, sqlite3 *db_connection) :
        _dbus_object(sdbus::createObject(dbus_connection, D_VCSTRACKER_OBJECT)),
        _database(db_connection) {

}

void dbus::VcsTracker::newRepo(std::string repo, std::string project) {

}

void dbus::VcsTracker::activityOnRepo(std::string repo) {

}
