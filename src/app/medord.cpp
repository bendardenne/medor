#include <iostream>
#include <csignal>
#include <unistd.h>
#include <pwd.h>

#include <boost/program_options.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <sdbus-c++/sdbus-c++.h>
#include <libnotify/notification.h>


#include "../include/dbus/Constants.h"
#include "../include/dbus/Tracker.h"


using namespace medor;
namespace po = boost::program_options;
namespace pt = boost::posix_time;

std::unique_ptr<sdbus::IConnection> connection;
bool running = true;

void signal_handler(int signum) {
    connection->leaveEventLoop();
    running = false;
}

int main(int argc, char **argv) {
    std::string homedir;

    // TODO we could look at XDG_CONFIG_DIR
    if ((homedir = getenv("HOME")).empty()) {
        homedir = getpwuid(getuid())->pw_dir;
    }

    // Options options to the user in the CLI
    po::options_description options("Options");
    options.add_options()
            ("help,h", "Show this help.")
            ("database,d", po::value<std::string>()->default_value(homedir + "/.config/medor/activities.db"),
             "Path to the database file.")
            ("daemon,D", "Fork to the backgroud.");

    po::variables_map vm;

    try {
        po::store(po::parse_command_line(argc, argv, options), vm);
    } catch (po::error &e) {
        std::cout << options << std::endl;
        std::cerr << e.what() << std::endl;
        return -EINVAL;
    }

    if (vm.count("help")) {
        std::cout << options << "\n";
        return 0;
    }

    if (vm.count("daemon")) {
        //TODO
    }

    connection = sdbus::createSystemBusConnection(D_SERVICE_NAME);

    // Start the tracker.
    dbus::Tracker tracker(*connection, vm["database"].as<std::string>());

    signal(SIGTERM, signal_handler);

    // Run the event loop asynchronously. We want to keep control to run some periodic checks.
    connection->enterEventLoopAsync();

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

                NotifyNotification *n = notify_notification_new("Time tracking",
                                                                ("You've been working on <b>" +
                                                                 status["project"].get<std::string>() +
                                                                 "</b> for <b>" + std::to_string(hoursOnProject) +
                                                                 "</b> hours.<br/>").c_str(),
                                                                NOTIFY_ICON);
                notify_notification_set_timeout(n, 5000);
                notify_notification_show(n, 0);
            }
        }
    }

    return 0;
}