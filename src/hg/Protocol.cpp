//
// Created by bdardenn on 4/28/20.
//

#include <algorithm>
#include <iostream>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>

#include "hg/Protocol.h"

using namespace medor;

hg::ChannelRecord hg::readRecord(int fd) {
    char buffer[4 * 1024];
    if ((read(fd, buffer, 4 * 1024)) > 0) {
        auto channel = static_cast<Channel>(buffer[0]);

        // Server uses big endian by specification.
        uint32_t length =
            (uint8_t)buffer[1] << 24u | (uint8_t)buffer[2] << 16u | (uint8_t)buffer[3] << 8u | (uint8_t)buffer[4];
        return {.channel = channel, .message = std::string(buffer + 5, length)};
    }

    throw ""; // TODO
}

std::vector<std::string> hg::runCommand(int fd, const std::string& command) {
    std::string rawCommand = "runcommand\n";
    std::string nullTerminated = command;
    std::replace(nullTerminated.begin(), nullTerminated.end(), ';', '\0');

    uint32_t commandLength = nullTerminated.size();
    uint32_t bigEndianSize = htonl(commandLength);

    write(fd, rawCommand.c_str(), rawCommand.size());
    write(fd, &bigEndianSize, 4);
    write(fd, nullTerminated.c_str(), nullTerminated.size());

    std::vector<std::string> result;
    ChannelRecord response;
    do {
        response = hg::readRecord(fd);
        switch (response.channel) {
        case Channel::Output:
            result.emplace_back(response.message);
            break;
        case Channel::Error:
            std::cerr << response.message << std::endl;
            // FIXME What to do here? Throw exception ?
        case Channel::Debug:
        case Channel::Input:
        case Channel::Line:
        default:
            break;
        }
    } while (response.channel != Channel::Result);

    return result;
}