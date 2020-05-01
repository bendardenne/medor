//
// Created by bdardenn on 4/29/20.
//

#include <boost/filesystem/operations.hpp>

#include "vcs/GitClient.h"
#include "vcs/HgClient.h"
#include "vcs/IVcsClient.h"

using namespace medor::vcs;

IVcsClient::~IVcsClient() {}

std::unique_ptr<IVcsClient> IVcsClient::create(const std::string& repo) {
    try {
        if (boost::filesystem::exists(boost::filesystem::path(repo + "/.git"))) {
            return std::make_unique<GitClient>(repo);
        }
        if (boost::filesystem::exists(boost::filesystem::path(repo + "/.hg"))) {
            return std::make_unique<HgClient>(repo);
        }
    } catch (std::runtime_error& e) {
        // Catch but don't fail, just don't return any client.
        return nullptr;
    }

    return nullptr;
};
