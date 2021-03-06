//
// Created by bdardenn on 4/29/20.
//

#include <boost/filesystem/operations.hpp>

#include "vcs/GitClient.hpp"
#include "vcs/HgClient.hpp"
#include "vcs/IVcsClient.hpp"

using namespace medor::vcs;

IVcsClient::~IVcsClient() {}

std::unique_ptr<IVcsClient> IVcsClient::create(const std::string& repo) {
    if (boost::filesystem::exists(boost::filesystem::path(repo + "/.git"))) {
        return std::make_unique<GitClient>(repo);
    }
    if (boost::filesystem::exists(boost::filesystem::path(repo + "/.hg"))) {
        return std::make_unique<HgClient>(repo);
    }

    return nullptr;
};
