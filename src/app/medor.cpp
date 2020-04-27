#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <pwd.h>
#include <sdbus-c++/sdbus-c++.h>

#include "dbus/CLI.h"

using namespace medor;
namespace po = boost::program_options;
namespace pt = boost::posix_time;

void start(dbus::CLI& cli, const std::vector<std::string>& arguments);
void status(dbus::CLI& cli, const std::vector<std::string>& arguments);
void report(dbus::CLI& cli, const std::vector<std::string>& arguments);

int main(int argc, char** argv) {
    // FIXME duplicated in medord
    std::string homedir;
    if ((homedir = getenv("HOME")).empty()) {
        homedir = getpwuid(getuid())->pw_dir;
    }

    std::string defaultDb = homedir + "/.config/medor/activities.db";

    po::options_description options("Options");
    // clang-format off
    options.add_options()
    ("help,h", "Show this help.")
    ("database,d",po::value<std::string>()->default_value(defaultDb),"Path to the database file.");
    // clang-format on

    po::options_description hiddenOptions("Hidden");
    // clang-format off
    hiddenOptions.add_options()
    ("command", po::value<std::string>(), "Command to execute.")
    ("subargs", po::value<std::vector<std::string> >(), "Arguments for command");
    // clang-format on

    po::positional_options_description pos;
    pos.add("command", 1).add("subargs", -1);

    po::variables_map vm;
    po::options_description allOptions;
    allOptions.add(options);
    allOptions.add(hiddenOptions);

    po::parsed_options parsed =
        po::command_line_parser(argc, argv).options(allOptions).positional(pos).allow_unregistered().run();
    po::store(parsed, vm);

    if (vm.count("help")) {
        std::cout << "medor [options] COMMAND ... " << std::endl;
        std::cout << options << std::endl;
        return EXIT_SUCCESS;
    }

    std::string dbFile = vm["database"].as<std::string>();
    sqlite3* dbConnection;
    sqlite3_open_v2(dbFile.c_str(), &dbConnection, SQLITE_OPEN_READONLY, nullptr);

    storage::ActivityStore activityStore(dbConnection);
    try {
        std::unique_ptr<sdbus::IConnection> dbusConnection = sdbus::createSessionBusConnection();
        dbus::CLI cli(activityStore, *dbusConnection);
        std::string cmd = vm["command"].as<std::string>();
        std::vector<std::string> unrecognized = po::collect_unrecognized(parsed.options, po::include_positional);
        unrecognized.erase(unrecognized.begin());

        if (cmd == "start") {
            start(cli, unrecognized);
        } else if (cmd == "stop") {
            cli.stop();
        } else if (cmd == "status") {
            status(cli, unrecognized);
        } else if (cmd == "resume") {
            cli.resume();
        } else if (cmd == "projects") {
            cli.projects();
        } else if (cmd == "report") {
//            report(cli, unrecognized);
            cli.report(pt::time_period(pt::second_clock::local_time(), pt::second_clock::local_time()));
        } else if (cmd == "quiet") {
            cli.setQuiet(std::string(argv[2]) == "on");
        } else if (cmd == "repo") {
            cli.addRepo(std::string(argv[2]), std::string(argv[3]));
        } else {
            std::cerr << "Unknown command: " << cmd << std::endl;
        }
    } catch (sdbus::Error& e) {
        std::cerr << e.getName() << ": " << e.getMessage() << std::endl;
        return -EXIT_FAILURE;
    }

    // TODO close db probably
    return EXIT_SUCCESS;
}

void start(dbus::CLI& cli, const std::vector<std::string>& arguments) {
    po::options_description options("start options");
    options.add_options()("project", "Project to be started");

    po::positional_options_description pos;
    pos.add("project", 1);

    po::variables_map vm;
    po::parsed_options parsed = po::command_line_parser(arguments).options(options).positional(pos).run();
    po::store(parsed, vm);
    cli.start(vm["project"].as<std::string>());
}

void status(dbus::CLI& cli, const std::vector<std::string>& arguments) {
    po::options_description options("start options");
    options.add_options()("format,f", po::value<std::string>()->default_value("%p"), "Output format");

    po::variables_map vm;
    po::parsed_options parsed = po::command_line_parser(arguments).options(options).run();
    po::store(parsed, vm);
    cli.status(vm["format"].as<std::string>());
}

void report(dbus::CLI& cli, const std::vector<std::string>& arguments) {
//    po::options_description options("start options");
//    options.add_options()("from", po::value<std::string>()->default_value("%p"), "Output format");
//    options.add_options()("to", po::value<std::string>()->default_value("%p"), "Output format");

//    po::variables_map vm;
//    po::parsed_options parsed = po::command_line_parser(arguments).options(options).run();
//    po::store(parsed, vm);
//    cli.status(vm["format"].as<std::string>());
}