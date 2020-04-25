//
// Created by bdardenn on 4/22/20.
//

#include "dbus/VcsTracker.h"

#include <utility>
#include "dbus/Constants.h"

using namespace medor;


dbus::VcsTracker::VcsTracker(sdbus::IConnection& dbusConnection, storage::VcsStore dbConnection)
    : _dbusObject(sdbus::createObject(dbusConnection, D_VCSTRACKER_OBJECT)), _vcsStore(dbConnection) {

    namespace ph = std::placeholders;

    std::function<void(std::string, std::string)> addRepo = std::bind(&VcsTracker::addRepo, this, ph::_1, ph::_2);

    _dbusObject->registerMethod("addRepo").onInterface(D_VCSTRACKER_INTERFACE).implementedAs(addRepo);
}

void dbus::VcsTracker::addRepo(std::string repo, std::string project) {
    _vcsStore.addRepo(std::move(repo), std::move(project));
    BOOST_LOG_SEV(_logger, Info) << "Added repo for project " + project + ": " + repo;
}

void dbus::VcsTracker::activityOnRepo(std::string repo) {
    BOOST_LOG_SEV(_logger, Info) << "Hint received from repo: " + repo;
}
