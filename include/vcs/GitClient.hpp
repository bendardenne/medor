//
// Created by bdardenn on 4/30/20.
//

#pragma once

#include <git2.h>

#include "IVcsClient.hpp"

namespace medor::vcs {

class GitClient : public IVcsClient {

  public:
    GitClient(const std::string& repoPath);

    ~GitClient();

    std::vector<LogEntry> log(pt::time_period date) override;

  private:
    git_repository* _repo;
};

} // namespace medor::vcs
