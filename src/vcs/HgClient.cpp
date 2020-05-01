//
// Created by bdardenn on 4/28/20.
//

#include <boost/algorithm/string.hpp>
#include <boost/date_time/period_formatter.hpp>
#include <boost/process/child.hpp>
#include <boost/process/search_path.hpp>
#include <iomanip>
#include <regex>
#include <sys/un.h>

#include "vcs/HgClient.h"
#include "vcs/HgProtocol.h"

namespace dt = boost::date_time;

using namespace medor::vcs;

HgClient::HgClient(const std::string& repoPath, const std::string& socketPath) {
    _child = proc::child(proc::search_path("hg"),
                         "serve",
                         "--cmdserver",
                         "pipe",
                         "--config",
                         "ui.interactive=False",
                         "-R",
                         repoPath,
                         "--cwd",
                         repoPath,
                         proc::std_out > _out,
                         proc::std_in < _in);

    // Read in welcome message.
    ChannelRecord record = vcs::readRecord(_out);
    // hg serve sends info about encoding, which we're not taking into account here.
}

HgClient::~HgClient() { _child.terminate(); }

std::vector<medor::vcs::LogEntry> HgClient::log(pt::time_period timePeriod) {
    std::string user = config()["ui.username"];
    std::stringstream command;
    command << "log;";
    command << "-u;" + user + ";";

    if (!timePeriod.is_null()) {
        dt::period_formatter<char> formatter(dt::period_formatter<char>::AS_CLOSED_RANGE, " to ", "", "", "");
        command << "-d;";
        formatter.put_period(command, command, ' ', timePeriod, pt::time_facet("%Y-%m-%d %H:%M:%S"));
        command << ";";
    }

    std::vector<LogEntry> result;
    std::regex field("(.+?):(.+)[\\r\\n]+");
    for (auto& line : vcs::runCommand(_out, _in, command.str())) {
        LogEntry current;
        auto fields = std::sregex_iterator(line.begin(), line.end(), field);

        for (std::sregex_iterator i = fields; i != std::sregex_iterator(); ++i) {
            std::smatch match = *i;
            std::string key = match[1].str();
            std::string value = match[2].str();
            boost::trim(value);
            if (key == "summary") {
                current.summary = value;
            } else if (key == "date") {
                std::stringstream ss(value);
                pt::ptime time;
                ss.imbue(std::locale(std::locale::classic(), new pt::time_input_facet("%a %b %d %H:%M:%S %Y %q")));
                ss >> time;
                current.date = time;
            }
        }

        result.emplace_back(current);
    }

    std::reverse(result.begin(), result.end());
    return result;
}

std::map<std::string, std::string> HgClient::config() {
    std::map<std::string, std::string> result;
    std::vector<std::string> lines = vcs::runCommand(_out, _in, "config");

    for (const auto& line : lines) {
        size_t index = line.find('=');
        std::string value = line.substr(index + 1);
        boost::trim(value); // Remove  \n
        result[line.substr(0, index)] = value;
    }

    return result;
}
