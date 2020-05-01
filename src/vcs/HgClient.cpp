//
// Created by bdardenn on 4/28/20.
//

#include <boost/algorithm/string.hpp>
#include <boost/date_time/period_formatter.hpp>
#include <csignal>
#include <iomanip>
#include <regex>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "vcs/HgClient.h"
#include "vcs/HgProtocol.h"

namespace dt = boost::date_time;

using namespace medor::vcs;
HgClient::HgClient(const std::string& repoPath, const std::string& socketPath) {
    std::string resolvedSocket;
    if (socketPath.empty()) {
        char tmpFileTemplate[] = "medorhg_XXXXXX";
        resolvedSocket = std::string(mktemp(tmpFileTemplate));
    } else {
        resolvedSocket = socketPath;
    }

    pid_t childPid = fork();
    if (childPid == -1) {
        BOOST_LOG_SEV(_logger, Critical) << "Could not fork process. Errno: " << errno;
    }

    // Spawn hg serve in the child process
    if (childPid == 0) {
        // HG serve prints some stuff on stdout. Redirect to /dev/null.
        freopen("/dev/null", "w", stdout);
        freopen("/dev/null", "w", stderr);
        execlp("hg",
               "hg",
               "serve",
               "--config",
               "ui.interactive=False",
               "--cmdserver",
               "unix",
               "--address",
               resolvedSocket.c_str(),
               "-R",
               repoPath.c_str(),
               nullptr);
        return;
    }

    //    int notifyFd = inotify_init();
    //    inotify_add_watch(notifyFd, resolvedSocket.c_str(), IN_CREATE)
    //
    //    // FIXME inotify wait for sock file
    //    sleep(1);

    _serverPid = childPid;
    _sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (_sockfd == -1) {
        BOOST_LOG_SEV(_logger, Error) << "Could not open socket. Errno: " << errno;
        throw std::runtime_error("Could not open socket for hg serve");
    }

    sockaddr_un addr = {
        .sun_family = AF_UNIX,
    };
    strncpy(addr.sun_path, resolvedSocket.c_str(), resolvedSocket.size());

    // TODO improve this
    int connection;
    while ((connection = connect(_sockfd, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr))) != 0) {
    }

    // Read in welcome message.
    ChannelRecord record = vcs::readRecord(_sockfd);
    // hg serve sends info about encoding, which we're not taking into account here.
}

HgClient::~HgClient() {
    close(_sockfd);
    kill(_serverPid, SIGTERM);
}

std::vector<medor::vcs::LogEntry> HgClient::log(pt::time_period timePeriod) {
    std::string user = config()["ui.username"];

    std::stringstream command;
    command << "log;";
    command << "-u;" << user << ";";

    if (!timePeriod.is_null()) {
        dt::period_formatter<char> formatter(dt::period_formatter<char>::AS_CLOSED_RANGE, " to ", "", "", "");
        command << "-d;";
        formatter.put_period(command, command, ' ', timePeriod, pt::time_facet("%Y-%m-%d %H:%M:%S"));
        command << ";";
    }

    std::vector<LogEntry> result;
    std::regex field("(.+?):(.+)[\\r\\n]+");
    for (auto& line : vcs::runCommand(_sockfd, command.str())) {
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
    std::vector<std::string> lines = vcs::runCommand(_sockfd, "config");

    for (const auto& line : lines) {
        size_t index = line.find('=');
        std::string value = line.substr(index + 1);
        boost::trim(value); // Remove  \n
        result[line.substr(0, index)] = value;
    }

    return result;
}
