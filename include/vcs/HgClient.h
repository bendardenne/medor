//
// Created by bdardenn on 4/28/20.
//

#pragma once

#include <map>
#include <string>
#include <vector>

#include "vcs/IVcsClient.h"

namespace medor::vcs {

class HgClient : public IVcsClient {

  public:
    HgClient(const std::string& repoPath, const std::string& socketPath = "");

    ~HgClient();

    std::vector<LogEntry> log(pt::time_period date) override;

    std::map<std::string, std::string> config();

  private:
    pid_t _serverPid;
    int _sockfd;
};

} // namespace medor::vcs
