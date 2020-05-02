//
// Created by bdardenn on 4/25/20.
//
#pragma once

#include <sdbus-c++/IConnection.h>

namespace medor::dbus {

/**
 * Class capable of sending system notifications.
 */
class Notifier {
  public:
    explicit Notifier(sdbus::IConnection& dbusConnection);

    /**
     * @return Whether notifications should be muted.
     */
    bool isQuiet() const;

    /**
     * @param quiet Whether notifications should be muted.
     */
    void setQuiet(bool quiet);

    /**
     * Send a notifications.
     * @param title             Title of the notification.
     * @param body              Body of the notification.
     * @param actionName        Optional name of an action to be linked to the notification.
     * @param label             User-friendly label for the optional action.
     * @param callback          Callback that will be called if the user activates the action.
     */
    void send(
        const std::string& title,
        const std::string& body,
        const std::string& actionName = "",
        const std::string& label = "",
        std::function<void()> callback = [] {});

  private:
    void actionHandler(unsigned int id, const std::string& actionName);

    std::map<std::string, std::function<void()>> _actionCallbacks;
    std::unique_ptr<sdbus::IProxy> _notifyProxy;
    bool _quiet;
};
} // namespace medor::dbus