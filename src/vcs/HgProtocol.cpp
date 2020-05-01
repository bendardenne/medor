//
// Created by bdardenn on 4/28/20.
//

#include <algorithm>
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>

#include "vcs/HgProtocol.h"

using namespace medor;

vcs::ChannelRecord vcs::readRecord(proc::ipstream& hgOut) {
    auto channel = static_cast<Channel>(hgOut.get());

    // Server uses big endian by specification.
    // TODO We can probably use ntohl or something
    uint32_t length =
        (uint8_t)hgOut.get() << 24u | (uint8_t)hgOut.get() << 16u | (uint8_t)hgOut.get() << 8u | (uint8_t)hgOut.get();

    char buffer[length];
    hgOut.read(buffer, length);
    return {.channel = channel, .message = std::string(buffer, length)};
}

std::vector<std::string> vcs::runCommand(proc::ipstream& hgOut, proc::opstream& hgIn, const std::string& command) {
    std::string rawCommand = "runcommand\n";
    std::string nullTerminated = command;
    uint32_t commandLength = nullTerminated.size();
    uint32_t bigEndianSize = htonl(commandLength);

    std::replace(nullTerminated.begin(), nullTerminated.end(), ';', '\0');

    hgIn << rawCommand;
    hgIn.write((const char*)&bigEndianSize, 4);
    hgIn.write(nullTerminated.c_str(), commandLength);
    hgIn.flush();

    std::vector<std::string> result;
    ChannelRecord response;
    do {
        response = vcs::readRecord(hgOut);
        switch (response.channel) {
        case Channel::Output:
            result.emplace_back(response.message);
            break;
        case Channel::Error:
            throw std::runtime_error(response.message); // Not sure if that's the best thing to do here.
        case Channel::Debug:
        case Channel::Input:
        case Channel::Line:
        default:
            break;
        }
    } while (response.channel != Channel::Result);

    return result;
}