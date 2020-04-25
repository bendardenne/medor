//
// Created by bdardenn on 4/25/20.
//
#pragma once

#include <giomm-2.4/giomm.h>
#include <glibmm-2.4/glibmm/refptr.h>

namespace medor::dbus {
class Notifier {
  public:
    Notifier();

    bool isQuiet() const;
    void setQuiet(bool quiet);
    void send(const std::string& title, const std::string& body);

  private:
    Glib::RefPtr<Gio::Application> _application;
    Glib::RefPtr<Gio::ThemedIcon> _icon;
    bool _quiet;
};
} // namespace medor::dbus