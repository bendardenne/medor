#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <pwd.h>
#include <sdbus-c++/sdbus-c++.h>

#include "dbus/CLI.h"

using namespace medor;
namespace po = boost::program_options;
namespace pt = boost::posix_time;

int main(int argc, char** argv) {
    // TODO better option parsing etc
    std::string cmd = argv[1];

    // FIXME duplicated in medord
    std::string homedir;
    if ((homedir = getenv("HOME")).empty()) {
        homedir = getpwuid(getuid())->pw_dir;
    }
    std::string dbFile = homedir + "/.config/medor/activities.db";

    sqlite3* dbConnection;
    sqlite3_open_v2(dbFile.c_str(), &dbConnection, SQLITE_OPEN_READONLY, nullptr);

    storage::ActivityStore activityStore(dbConnection);
    try {
        dbus::CLI cli(activityStore);

        if (cmd == "start") {
            cli.start(argv[2]);
        } else if (cmd == "stop") {
            cli.stop();
        } else if (cmd == "status") {
            std::string format = argc >= 3 ? argv[2] : "%p";
            cli.status(format);
        } else if (cmd == "resume") {
            cli.resume();
        } else if (cmd == "projects") {
            cli.projects();
        } else if (cmd == "report") {
            // TODO get from arguments somehow
            cli.report(pt::time_period(pt::second_clock::local_time(), pt::second_clock::local_time()));
        } else if (cmd == "quiet") {
            cli.setQuiet(std::string(argv[2]) == "on");
        }
    } catch (sdbus::Error& e) {
        std::cerr << e.getName() << ": " << e.getMessage() << std::endl;
        return -EXIT_FAILURE;
    }

    return 0;
}
