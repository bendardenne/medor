//
// Created by bdardenn on 4/22/20.
//

#include "dbus/VcsHinter.hpp"

#include "dbus/Constants.hpp"
#include <utility>

using namespace medor;

dbus::VcsHinter::VcsHinter(sdbus::IConnection& dbusConnection,
                           std::shared_ptr<Notifier> notifier,
                           std::shared_ptr<Tracker> tracker,
                           std::shared_ptr<storage::VcsStore> vcsStore,
                           std::shared_ptr<storage::ProjectStore> projectStore)
    : _dbusObject(sdbus::createObject(dbusConnection, D_VCSHINTER_OBJECT)), _tracker(std::move(tracker)),
      _notifier(std::move(notifier)), _vcsStore(std::move(vcsStore)), _projectStore(std::move(projectStore)) {

    namespace ph = std::placeholders;

    std::function<void(std::string, std::string)> addRepo = std::bind(&VcsHinter::addRepo, this, ph::_1, ph::_2);
    std::function<void(std::string, std::string)> removeRepo = std::bind(&VcsHinter::removeRepo, this, ph::_1, ph::_2);
    std::function<void(std::string)> activityOnRepo = std::bind(&VcsHinter::activityOnRepo, this, ph::_1);

    _dbusObject->registerMethod("addRepo").onInterface(D_VCSHINTER_INTERFACE).implementedAs(addRepo);
    _dbusObject->registerMethod("removeRepo").onInterface(D_VCSHINTER_INTERFACE).implementedAs(removeRepo);
    _dbusObject->registerMethod("activityOnRepo").onInterface(D_VCSHINTER_INTERFACE).implementedAs(activityOnRepo);

    _dbusObject->finishRegistration();
}

void dbus::VcsHinter::addRepo(const std::string& project, const std::string& repo) {
    int projectId = _projectStore->getIdForProject(project);
    bool added = _vcsStore->addRepo(repo, {.id = projectId, .name = project});

    if (!added) {
        throw sdbus::Error(std::string(D_VCSHINTER_INTERFACE) + ".Error", "Could not add repo. Already registered?");
        BOOST_LOG_SEV(_logger, Warning) << "Could not add repo for project " << project << ": " << repo;
    } else {
        BOOST_LOG_SEV(_logger, Info) << "Added repo for project " << project << ": " << repo;
    }
}

void dbus::VcsHinter::removeRepo(const std::string& project, const std::string& repo) {
    int projectId = _projectStore->getIdForProject(project);
    bool removed = _vcsStore->removeRepo(repo, {.id = projectId, .name = project});

    if (!removed) {
        throw sdbus::Error(std::string(D_VCSHINTER_INTERFACE) + ".Error", "Could not remove repo");
        BOOST_LOG_SEV(_logger, Warning) << "Could not remove repo for project " << project << ": " << repo;
    } else {
        BOOST_LOG_SEV(_logger, Info) << "Removing repo for project " << project << ": " << repo;
    }
}

void dbus::VcsHinter::activityOnRepo(const std::string& repo) {
    BOOST_LOG_SEV(_logger, Info) << "Hint received from repo: " + repo;
    auto status = _tracker->status();

    std::optional<int> projectId = _vcsStore->getReposFor(repo);

    if (!projectId) {
        BOOST_LOG_SEV(_logger, Info) << "Hinted repo is not yet known: " << repo;
        if (status.count("project")) {
            auto projectName = status["project"].get<std::string>();
            auto addToCurrent = [this, projectName, repo]() { addRepo(projectName, repo); };

            _notifier->send(
                "New repo detected", "Link to <b> " + projectName + "</b>?", "addToCurrent", "Yes", addToCurrent);
        }

        return;
    }

    if (status.count("project_id") > 0 && projectId.value() == status["project_id"].get<int>()) {
        BOOST_LOG_SEV(_logger, Info) << "Hinted project is already being tracked "
                                     << status["project"].get<std::string>();
        return; // We are already tracking the associated project. Nothing to do.
    }

    std::string projectName = _projectStore->getProjectForId(projectId.value()).value().name;
    BOOST_LOG_SEV(_logger, Warning) << "Hinted project is not yet being tracked:" << projectName;
    auto startProject = [this, projectName]() { _tracker->start(projectName); };
    _notifier->send(
        "Activity detected", "Are you working on <b> " + projectName + "</b>?", "startProject", "Yes", startProject);
}
