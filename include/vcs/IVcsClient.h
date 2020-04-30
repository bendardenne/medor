//
// Created by bdardenn on 4/29/20.
//

#pragma once

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>

#include "util/logging.h"

namespace pt = boost::posix_time;
namespace logsrc = boost::log::sources;

namespace medor::vcs {

struct LogEntry {
    std::string summary;
    pt::ptime date;
};

class IVcsClient {

  public:
    virtual ~IVcsClient() = 0;

    virtual std::vector<LogEntry> log(pt::time_period date) = 0;

    static std::unique_ptr<IVcsClient> create(const std::string& repo);

  protected:
    logsrc::severity_logger<Severity> _logger;
};

} // namespace medor::vcs
