#include <csignal>
#include <iostream>
#include <pwd.h>
#include <unistd.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <libnotify/notification.h>
#include <sdbus-c++/sdbus-c++.h>

#include "dbus/Constants.h"
#include "dbus/Tracker.h"
#include "dbus/VcsTracker.h"

using namespace medor;
namespace po = boost::program_options;
namespace pt = boost::posix_time;
namespace fs = boost::filesystem;

std::unique_ptr<sdbus::IConnection> dbus_connection;
bool running = true;

void signal_handler(int signum) {
    dbus_connection->leaveEventLoop();
    running = false;
}

int main(int argc, char** argv) {
    std::string homedir;

    // TODO we could look at XDG_CONFIG_DIR
    if ((homedir = getenv("HOME")).empty()) {
        homedir = getpwuid(getuid())->pw_dir;
    }

    // Options options to the user in the CLI
    po::options_description options("Options");
    options.add_options()("help,h", "Show this help.")(
        "database,d", po::value<std::string>()->default_value(homedir + "/.config/medor/activities.db"),
        "Path to the database file.")("daemon,D", "Fork to the backgroud.");

    po::variables_map vm;

    try {
        po::store(po::parse_command_line(argc, argv, options), vm);
    } catch (po::error& e) {
        std::cout << options << std::endl;
        std::cerr << e.what() << std::endl;
        return -EINVAL;
    }

    if (vm.count("help")) {
        std::cout << options << "\n";
        return 0;
    }

    if (vm.count("daemon")) {
        // TODO
    }

    dbus_connection = sdbus::createSystemBusConnection(D_SERVICE_NAME);

    std::string database_file = vm["database"].as<std::string>();

    fs::path path(database_file);
    fs::create_directories(path.parent_path());

    sqlite3* db_connection;
    sqlite3_open_v2(database_file.c_str(), &db_connection, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);
    // Start the tracker.
    dbus::Tracker tracker(*dbus_connection, db_connection);
    dbus::VcsTracker vcsTracker(*dbus_connection, db_connection);

    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);

    // Run the event loop asynchronously. We want to keep control to run some
    // periodic checks.
    dbus_connection->enterEventLoopAsync();

    unsigned int hoursOnProject = 0;
    while (running) {
        sleep(5);
        auto status = tracker.status();
        if (status.count("project") > 0) {
            pt::ptime start = pt::from_iso_string(status["start"].get<std::string>());
            pt::ptime now = pt::second_clock::local_time();
            pt::time_period period(start, now);

            if (period.length().hours() > hoursOnProject) {
                hoursOnProject = period.length().hours();

                NotifyNotification* n =
                    notify_notification_new("Time tracking",
                                            ("You've been working on <b>" + status["project"].get<std::string>() +
                                             "</b> for <b>" + std::to_string(hoursOnProject) + "</b> hours.<br/>")
                                                .c_str(),
                                            NOTIFY_ICON);
                notify_notification_set_timeout(n, 5000);
                notify_notification_show(n, 0);
            }
        }
    }

    int ret = sqlite3_close(db_connection);

    return 0;
}