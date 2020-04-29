//
// Created by bdardenn on 4/28/20.
//

#pragma once

#include <string>
namespace medor::vcs {

enum class Channel : char { Line = 'L', Input = 'I', Output = 'o', Result = 'r', Error = 'e', Debug = 'd' };

struct ChannelRecord {
    Channel channel;
    std::string message;
};

ChannelRecord readRecord(int fd);

std::vector<std::string> runCommand(int fd, const std::string& command);

} // namespace medor::vcs