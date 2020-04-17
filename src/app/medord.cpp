#include <iostream>
#include <csignal>
#include <unistd.h>
#include <pwd.h>

#include <boost/program_options.hpp>
#include <sdbus-c++/sdbus-c++.h>


#include "../include/dbus/Constants.h"
#include "../include/dbus/Tracker.h"


using namespace medor;
namespace po = boost::program_options;

std::unique_ptr<sdbus::IConnection> connection;

void signal_handler(int signum) {
    std::cout << "TERM" << std::endl;
    connection->leaveEventLoop();
//    exit(signum);
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

    // Run the I/O event loop on the bus connection.
    connection->enterEventLoop();
    return 0;
}