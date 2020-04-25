//
// Created by bdardenn on 4/25/20.
//

#include "dbus/Notifier.h"
#include "dbus/Constants.h"

using namespace medor::dbus;

Notifier::Notifier()
    : _application(Gio::Application::create(D_SERVICE_NAME, Gio::APPLICATION_FLAGS_NONE)),
      _icon(Gio::ThemedIcon::create("clock")) {
    _application->register_application();
}

bool Notifier::isQuiet() const { return _quiet; }

void Notifier::setQuiet(bool quiet) { _quiet = quiet; }

void Notifier::send(const std::string& title, const std::string& body) {
    if (isQuiet()) {
        return;
    }

    auto notification = Gio::Notification::create(title);
    notification->set_body(body);
    notification->set_icon(_icon);
    _application->send_notification(notification);
}
