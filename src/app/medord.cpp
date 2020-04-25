#include <csignal>
#include <iostream>
#include <pwd.h>
#include <string>
#include <unistd.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/filesystem.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>
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
namespace logging = boost::log;

std::unique_ptr<sdbus::IConnection> dbusConnection;
bool running = true;

void initLogging(const std::string& logFile) {
    logging::add_common_attributes();
    logging::add_file_log(logFile, logging::keywords::format = "[%TimeStamp%]: %Message%");
}

void signalHandler(int signum) {
    dbusConnection->leaveEventLoop();
    running = false;
}

int main(int argc, char** argv) {
    std::string homedir;

    // TODO we could look at XDG_CONFIG_DIR
    if ((homedir = getenv("HOME")).empty()) {
        homedir = getpwuid(getuid())->pw_dir;
    }

    initLogging(homedir + "/.config/medor/medord.log");

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

    dbusConnection = sdbus::createSystemBusConnection(D_SERVICE_NAME);

    std::string databaseFile = vm["database"].as<std::string>();

    fs::path path(databaseFile);
    fs::create_directories(path.parent_path());

    sqlite3* dbConnection;
    sqlite3_open_v2(databaseFile.c_str(), &dbConnection, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, nullptr);

    storage::ActivityStore activityStore(dbConnection);
    storage::VcsStore vcsStore(dbConnection);

    // Start the tracker.
    dbus::Tracker tracker(*dbusConnection, activityStore);
    dbus::VcsTracker vcsTracker(*dbusConnection, vcsStore);

    signal(SIGTERM, signalHandler);
    signal(SIGINT, signalHandler);

    // Run the event loop asynchronously. We want to keep control to run some
    // periodic checks.
    dbusConnection->enterEventLoopAsync();

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

    int ret = sqlite3_close(dbConnection);

    return 0;
}