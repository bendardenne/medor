#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <pwd.h>
#include <sdbus-c++/sdbus-c++.h>

#include "dbus/CLI.h"
#include "util/time.h"

using namespace medor;
namespace po = boost::program_options;
namespace pt = boost::posix_time;

int doCommand(po::variables_map vm, std::vector<char> parsedOptions);

void start(dbus::CLI& cli, const std::vector<std::string>& arguments);
void stop(dbus::CLI& cli, const std::vector<std::string>& arguments);
void resume(dbus::CLI& cli, const std::vector<std::string>& arguments);
void status(dbus::CLI& cli, const std::vector<std::string>& arguments);
void report(dbus::CLI& cli, const std::vector<std::string>& arguments);
void quiet(dbus::CLI& cli, const std::vector<std::string>& arguments);
void repo(dbus::CLI& cli, const std::vector<std::string>& arguments);

int main(int argc, char** argv) {
    std::unordered_map<std::string, std::function<void(dbus::CLI&, const std::vector<std::string>&)>> commands = {
        {"start", &start},
        {"stop", &stop},
        {"resume", &resume},
        {"status", &status},
        {"report", &report},
        {"quiet", &quiet},
        {"repo", &repo},
    };

    // FIXME duplicated in medord
    std::string homedir;
    if ((homedir = getenv("HOME")).empty()) {
        homedir = getpwuid(getuid())->pw_dir;
    }

    std::string defaultDb = homedir + "/.config/medor/activities.db";
    po::options_description options;

    // clang-format off
    options.add_options()
        ("help,h","Show this help.")
        ("database,d",boost::program_options::value<std::string>()->default_value(defaultDb),"Path to the database file.");
    // clang-format on

    po::options_description hiddenOptions("Hidden");
    // clang-format off
    hiddenOptions.add_options()
        ("command", boost::program_options::value<std::string>(), "Command to execute.")
        ("subargs", boost::program_options::value<std::vector<std::string> >(), "Arguments for command");
    // clang-format on

    po::positional_options_description pos;
    pos.add("command", 1).add("subargs", -1);

    po::options_description allOptions;
    allOptions.add(options);
    allOptions.add(hiddenOptions);

    po::parsed_options parsed =
        po::command_line_parser(argc, argv).options(allOptions).positional(pos).allow_unregistered().run();

    po::variables_map vm;
    po::store(parsed, vm);

    if (vm.count("help") > 0 || vm.count("command") == 0) {
        std::cout << "medor [options] COMMAND ... " << std::endl;

        std::cout << "Commands: ";
        for (auto key : commands) {
            std::cout << key.first << " ";
        }
        std::cout << std::endl;
        std::cout << options << std::endl;
        return EXIT_SUCCESS;
    }
    std::string cmd = vm["command"].as<std::string>();
    std::vector<std::string> unrecognized = po::collect_unrecognized(parsed.options, po::include_positional);
    // Remove "command" from leftover unparsed arguments.
    unrecognized.erase(std::find(unrecognized.begin(), unrecognized.end(), cmd));

    try {
        std::string dbFile = vm["database"].as<std::string>();
        sqlite3* dbConnection;
        sqlite3_open_v2(dbFile.c_str(), &dbConnection, SQLITE_OPEN_READONLY, nullptr);

        storage::ActivityStore activityStore(dbConnection);
        std::unique_ptr<sdbus::IConnection> dbusConnection = sdbus::createSessionBusConnection();
        dbus::CLI cli(activityStore, *dbusConnection);

        if (commands.count(cmd) == 0) {
            std::cerr << "No such command: " << cmd << std::endl;
            return EXIT_FAILURE;
        }

        commands[cmd](cli, unrecognized);
    } catch (sdbus::Error& e) {
        std::cerr << e.getName() << ": " << e.getMessage() << std::endl;
        return EXIT_FAILURE;
    }

    // TODO close db properly
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

void stop(dbus::CLI& cli, const std::vector<std::string>& arguments) { cli.stop(); }

void resume(dbus::CLI& cli, const std::vector<std::string>& arguments) { cli.resume(); }

void status(dbus::CLI& cli, const std::vector<std::string>& arguments) {
    po::options_description options("start options");
    options.add_options()("format,f", po::value<std::string>()->default_value("%p"), "Output format");

    po::variables_map vm;
    po::parsed_options parsed = po::command_line_parser(arguments).options(options).run();
    po::store(parsed, vm);
    cli.status(vm["format"].as<std::string>());
}

void report(dbus::CLI& cli, const std::vector<std::string>& arguments) {
    // TODO maybe allow more flexibility, like arbitrary from and to dates?
    po::options_description options("report options");
    options.add_options()("week",
                          po::value<int>()->default_value(0),
                          "Week for which we want a report. 0 = this week, -1 = last week, etc.");

    po::positional_options_description pos;
    pos.add("week", 1);

    po::variables_map vm;
    po::parsed_options parsed = po::command_line_parser(arguments).options(options).positional(pos).run();
    po::store(parsed, vm);
    pt::time_period period = util::time::weekFromNow(vm["week"].as<int>());
    cli.report(period);
}

void quiet(dbus::CLI& cli, const std::vector<std::string>& arguments) {
    // TODO maybe allow more flexibility, like arbitrary from and to dates?
    po::options_description options("quiet options");
    options.add_options()("value", po::value<std::string>(), "'on' or 'off'");

    po::positional_options_description pos;
    pos.add("value", 1);

    po::variables_map vm;
    po::parsed_options parsed = po::command_line_parser(arguments).options(options).positional(pos).run();
    po::store(parsed, vm);

    if (vm.count("value") == 0) {
        std::cout << "quiet: " << (cli.isQuiet() ? "on" : "off") << std::endl;
        return;
    }

    std::string value = vm["value"].as<std::string>();

    if (value == "on" or value == "true") {
        cli.setQuiet(true);
    } else if (value == "off" or value == "false") {
        cli.setQuiet(false);
    } else {
        std::cerr << "Unknown value: " + value << std::endl;
    }
}

void repo(dbus::CLI& cli, const std::vector<std::string>& arguments) {
    //    po::options_description options("quiet options");
    //    options.add_options()
    //    ("project", po::value<std::string>(), "'on' or 'off'")
    //    ("path", po::value<std::string>(), "'on' or 'off'");
    //
    //    po::positional_options_description pos;
    //    pos.add("command", 1);
    //    pos.add("path", 1);
    //
    //    po::variables_map vm;
    //    po::parsed_options parsed = po::command_line_parser(arguments).options(options).positional(pos).run();
    //    po::store(parsed, vm);
    //
    //    if (vm.count("value") == 0) {
    //        std::cout << "quiet: " << (cli.isQuiet() ? "on" : "off") << std::endl;
    //        return;
    //    }
    //
    //    std::string value = vm["value"].as<std::string>();
    //
    //    if (value == "on" or value == "true") {
    //        cli.setQuiet(true);
    //    } else if (value == "off" or value == "false") {
    //        cli.setQuiet(false);
    //    } else {
    //        std::cerr << "Unknown value: " + value << std::endl;
    //    }
}