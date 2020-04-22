//
// Created by bdardenn on 4/19/20.
//

#ifndef MEDOR_TIMEUTIL_H
#define MEDOR_TIMEUTIL_H

#include <boost/date_time/posix_time/posix_time.hpp>

namespace medor::util::time {
namespace pt = boost::posix_time;
namespace greg = boost::gregorian;

/**
 * Creates a time period of a given week, relative to the current week.
 * @param offset 0 = current week. 1 = last week. 2 = 2 weeks ago. etc.
 */
pt::time_period week_from_now(unsigned int offset);

/**
 * Formats duration in a human-readable way
 */
std::string format_duration(const pt::time_duration& duration, bool brief);

pt::time_duration aggregateTimes(const std::vector<model::Activity>& activities);

} // namespace medor::util::time

#endif // MEDOR_TIMEUTIL_H
