//
// Created by bdardenn on 4/29/20.
//

#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>

namespace pt = boost::posix_time;

namespace medor::vcs {

struct LogEntry {
    std::string summary;
    pt::ptime date;
};

class IVcsClient {

  public:
    virtual std::vector<LogEntry> log(pt::time_period date) = 0;

    static std::unique_ptr<IVcsClient> create(const std::string& repo);
};

} // namespace medor::vcs
