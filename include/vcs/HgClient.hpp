//
// Created by bdardenn on 4/28/20.
//

#pragma once

#include <boost/process.hpp>
#include <map>
#include <string>
#include <vector>

#include "vcs/IVcsClient.hpp"

namespace proc = boost::process;

namespace medor::vcs {

class HgClient : public IVcsClient {

  public:
    HgClient(const std::string& repoPath, const std::string& socketPath = "");

    ~HgClient();

    std::vector<LogEntry> log(pt::time_period date) override;

    std::map<std::string, std::string> config();

  private:
    proc::child _child;
    proc::ipstream _out;
    proc::opstream _in;
};

} // namespace medor::vcs
