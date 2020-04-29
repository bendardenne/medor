#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <pwd.h>
#include <sdbus-c++/sdbus-c++.h>
#include <util/database.h>

#include "dbus/CLI.h"
#include "util/time.h"

using namespace medor;
namespace po = boost::program_options;
namespace pt = boost::posix_time;
namespace fs = boost::filesystem;

void start(dbus::CLI& cli, const std::vector<std::string>& arguments);
void stop(dbus::CLI& cli, const std::vector<std::string>& arguments);
void resume(dbus::CLI& cli, const std::vector<std::string>& arguments);
void status(dbus::CLI& cli, const std::vector<std::string>& arguments);
void report(dbus::CLI& cli, const std::vector<std::string>& arguments);
void quiet(dbus::CLI& cli, const std::vector<std::string>& arguments);
void repo(dbus::CLI& cli, const std::vector<std::string>& arguments);
void projects(dbus::CLI& cli, const std::vector<std::string>& arguments);

int main(int argc, char** argv) {
    std::map<std::string, std::function<void(dbus::CLI&, const std::vector<std::string>&)>> commands = {
        {"start", &start},
        {"stop", &stop},
        {"resume", &resume},
        {"status", &status},
        {"report", &report},
        {"quiet", &quiet},
        {"repo", &repo},
        {"projects", &projects},
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

    // Collect all non-known options into positional arguments.
    po::options_description hiddenOptions("Hidden");
    // clang-format off
    hiddenOptions.add_options()
        ("unparsed", boost::program_options::value<std::vector<std::string> >(), "Arguments for command");
    // clang-format on
    po::positional_options_description pos;
    pos.add("unparsed", -1);

    po::options_description allOptions;
    allOptions.add(options);
    allOptions.add(hiddenOptions);

    po::parsed_options parsed =
        po::command_line_parser(argc, argv).options(allOptions).positional(pos).allow_unregistered().run();

    po::variables_map vm;
    po::store(parsed, vm);

    // Look for any known command.
    std::vector<std::string> unrecognized = po::collect_unrecognized(parsed.options, po::include_positional);
    std::string cmd;
    for (const auto& candidate : unrecognized) {
        if (commands.count(candidate)) {
            cmd = candidate;
            break;
        }
    }

    // If -h given, or no known command is given, show help.
    if (vm.count("help") > 0 || cmd.empty()) {
        std::cout << "medor [options] COMMAND ... " << std::endl;

        std::cout << "Commands: ";
        // Unfortunately, this comes in a weird order. Consider making a string of this.
        for (const auto& key : commands) {
            std::cout << key.first << " ";
        }
        std::cout << std::endl;
        std::cout << options << std::endl;
        return EXIT_SUCCESS;
    }

    // At this point we have a known command.
    // Remove "command" from leftover unparsed arguments.
    unrecognized.erase(std::find(unrecognized.begin(), unrecognized.end(), cmd));

    sqlite3* dbConnection;
    try {
        // Setup all the stuff we need.
        std::string dbFile = vm["database"].as<std::string>();
        sqlite3_open_v2(dbFile.c_str(), &dbConnection, SQLITE_OPEN_READONLY, nullptr);

        storage::ActivityStore activityStore(dbConnection);
        storage::VcsStore vcsStore(dbConnection);
        std::unique_ptr<sdbus::IConnection> dbusConnection = sdbus::createSessionBusConnection();
        dbus::CLI cli(activityStore, vcsStore, *dbusConnection);

        // Call command callback.
        commands[cmd](cli, unrecognized);
    } catch (sdbus::Error& e) {
        std::cerr << e.getName() << ": " << e.getMessage() << std::endl;
        return EXIT_FAILURE;
    }

    util::database::checkError(sqlite3_close(dbConnection));
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
                          "Week for which we want a report. 0 = this week, 1 = last week, etc.");

    po::positional_options_description pos;
    pos.add("week", 1);

    po::variables_map vm;
    po::parsed_options parsed = po::command_line_parser(arguments).options(options).positional(pos).run();
    po::store(parsed, vm);
    pt::time_period period = util::time::weekFromNow(vm["week"].as<int>());
    cli.report(period);
}

void quiet(dbus::CLI& cli, const std::vector<std::string>& arguments) {
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
    po::options_description options("repo options");
    options.add_options()("project", po::value<std::string>(), "Project")(
        "path", po::value<std::string>(), "Repo path");

    po::positional_options_description pos;
    pos.add("path", 1);
    pos.add("project", 1);

    po::variables_map vm;
    po::parsed_options parsed = po::command_line_parser(arguments).options(options).positional(pos).run();
    po::store(parsed, vm);

    fs::path path;
    if (!vm.count("path")) {
        std::cerr << "Repo path is mandatory" << std::endl;
        return;
    }

    try {
        path = fs::canonical(fs::path(vm["path"].as<std::string>()));
    } catch (fs::filesystem_error& e) {
        std::cerr << "Not a valid repo path: " << vm["path"].as<std::string>() << std::endl;
        return;
    }

    if (vm.count("path") && vm.count("project")) {
        cli.addRepo(vm["project"].as<std::string>(), path.string());
        return;
    }

    cli.activityOnRepo(path.string());
}

void projects(dbus::CLI& cli, const std::vector<std::string>& arguments) {
    po::options_description options("projects options");
    options.add_options()(
        "limit,l", po::value<unsigned int>()->required()->default_value(50), "How many items to return at the most");

    po::variables_map vm;
    po::parsed_options parsed = po::command_line_parser(arguments).options(options).run();
    po::store(parsed, vm);

    cli.projects(vm["limit"].as<unsigned int>());
}
