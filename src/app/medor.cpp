#include <iostream>
#include <boost/program_options.hpp>
#include <sdbus-c++/sdbus-c++.h>
#include <pwd.h>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include "dbus/CLI.h"

void parseCommandOptions(boost::program_options::parsed_options options,
                         boost::program_options::options_description description,
                         boost::program_options::variables_map map);

using namespace medor;
namespace po = boost::program_options;
namespace pt = boost::posix_time;

int main(int argc, char **argv) {
    // TODO better option parsing etc
    std::string cmd = argv[1];

    // FIXME duplicated in medord
    std::string homedir;
    if ((homedir = getenv("HOME")).empty()) {
        homedir = getpwuid(getuid())->pw_dir;
    }
    std::string db_file = homedir + "/.config/medor/activities.db";

    try {
        dbus::CLI cli(db_file);

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
    } catch (sdbus::Error &e) {
        std::cerr << "Medor is not running" << std::endl;
        return -EHOSTDOWN;
    }


    return 0;
}







