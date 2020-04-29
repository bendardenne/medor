//
// Created by bdardenn on 4/29/20.
//
#include "vcs/IVcsClient.h"
#include "vcs/HgClient.h"

using namespace medor::vcs;
std::unique_ptr<IVcsClient> IVcsClient::create(const std::string& repo) { return std::make_unique<HgClient>(repo); }
