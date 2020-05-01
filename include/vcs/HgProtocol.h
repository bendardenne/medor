//
// Created by bdardenn on 4/28/20.
//

#pragma once
#include <boost/process/pipe.hpp>
#include <string>

namespace proc = boost::process;

namespace medor::vcs {

enum class Channel : char { Line = 'L', Input = 'I', Output = 'o', Result = 'r', Error = 'e', Debug = 'd' };

struct ChannelRecord {
    Channel channel;
    std::string message;
};

ChannelRecord readRecord(proc::ipstream& hgOut);

std::vector<std::string> runCommand(proc::ipstream& hgOut, proc::opstream& hgIn, const std::string& command);

} // namespace medor::vcs