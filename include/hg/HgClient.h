//
// Created by bdardenn on 4/28/20.
//

#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>
#include <map>
#include <string>
#include <vector>

namespace pt = boost::posix_time;

namespace medor::hg {

struct LogEntry {
    int revisionNumber;
    std::string changeset;
    std::string summary;
    std::string user;
    pt::ptime date;
    std::vector<std::string> tags;
};

class HgClient {

  public:
    HgClient(const std::string& repoPath, const std::string& socketPath = "");

    ~HgClient();

    std::vector<LogEntry> log();

    std::vector<LogEntry> log(const std::string& user, pt::time_period date);

    std::map<std::string, std::string> config();

  private:
    pid_t _serverPid;
    int _sockfd;
};

} // namespace medor::hg
