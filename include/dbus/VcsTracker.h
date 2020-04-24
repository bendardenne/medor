//
// Created by bdardenn on 4/22/20.
//

#ifndef MEDOR_VCSTRACKER_H
#define MEDOR_VCSTRACKER_H

#include <sdbus-c++/sdbus-c++.h>
#include <sqlite3.h>
#include <string>

#include "storage/VcsStore.h"

using namespace medor;

namespace medor::dbus {

class VcsTracker {

  public:
    VcsTracker(sdbus::IConnection& dbusConnection, storage::VcsStore dbConnection);

  private:
    void addRepo(std::string repo, std::string project);

    void activityOnRepo(std::string repo);

    storage::VcsStore _vcsStore;
    std::unique_ptr<sdbus::IObject> _dbusObject{};
};
} // namespace medor::dbus

#endif // MEDOR_VCSTRACKER_H
