//
// Created by bdardenn on 4/25/20.
//

#include <utility>
#include <sdbus-c++/IProxy.h>

#include "dbus/Constants.h"
#include "dbus/Notifier.h"

using namespace medor::dbus;

Notifier::Notifier(sdbus::IConnection& dbusConnection)
    : _notifyProxy(sdbus::createProxy(dbusConnection, D_NOTIF_INTERFACE, D_NOTIF_OBJECT)), _quiet(false) {

    std::function<void(unsigned int, const std::string&)> actionHandler =
        std::bind(&Notifier::actionHandler, this, std::placeholders::_1, std::placeholders::_2);

    _notifyProxy->uponSignal("ActionInvoked").onInterface(D_NOTIF_INTERFACE).call(actionHandler);
    _notifyProxy->finishRegistration();
}

bool Notifier::isQuiet() const { return _quiet; }

void Notifier::setQuiet(bool quiet) { _quiet = quiet; }

void Notifier::send(const std::string& title,
                    const std::string& body,
                    const std::string& actionName,
                    const std::string& label,
                    std::function<void()> callback) {
    if (isQuiet()) {
        return;
    }

    std::vector<std::string> actions;
    if (!actionName.empty()) {
        _actionCallbacks[actionName] = std::move(callback);
        actions.emplace_back(actionName);
        actions.emplace_back(label);
    }

    _notifyProxy->callMethod("Notify")
        .onInterface(D_NOTIF_INTERFACE)
        .withArguments(
            "medor", (unsigned int)0, "clock", title, body, actions, std::map<std::string, sdbus::Variant>(), -1);
}

void Notifier::actionHandler(unsigned int id, const std::string& actionName) {
    if (_actionCallbacks.count(actionName) == 0) {
        // This should normally not happen.
        return;
    }

    _actionCallbacks[actionName]();
    _actionCallbacks.erase(actionName);
}