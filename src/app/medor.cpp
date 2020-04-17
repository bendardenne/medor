#include <iostream>
#include <boost/program_options.hpp>
#include <sdbus-c++/sdbus-c++.h>

#include "dbus/CLI.h"

void parseCommandOptions(boost::program_options::parsed_options options,
                         boost::program_options::options_description description,
                         boost::program_options::variables_map map);

using namespace medor;
namespace po = boost::program_options;

int main(int argc, char **argv) {
    // TODO better option parsing etc
    std::string cmd = argv[1];

    try {
        dbus::CLI cli;

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
        }
    } catch (sdbus::Error &e) {
        std::cerr << "Medor is not running" << std::endl;
        return -EHOSTDOWN;
    }


    return 0;
}







