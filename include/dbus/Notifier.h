//
// Created by bdardenn on 4/25/20.
//
#pragma once

#include <sdbus-c++/IConnection.h>

namespace medor::dbus {
class Notifier {
  public:
    explicit Notifier(sdbus::IConnection& dbusConnection);

    bool isQuiet() const;
    void setQuiet(bool quiet);
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