//
// Created by bdardenn on 4/19/20.
//

#include "util/time.h"

//using namespace medor::util;

using namespace medor::util;
namespace pt = boost::posix_time;

pt::time_period time::week_from_now(unsigned int offset) {
    // Generators that give the first Monday (Sunday) before (after) a given date.
    greg::first_day_of_the_week_before monday(greg::Monday);
    greg::first_day_of_the_week_after sunday(greg::Sunday);

    // Go back offset amount of weeks
    greg::date now = greg::day_clock::local_day() - greg::date_duration(7 * offset);

    pt::ptime week_start = pt::ptime(monday.get_date(now), pt::hours(0));
    pt::ptime week_end = pt::ptime(sunday.get_date(now), pt::hours(23) + pt::minutes(59) + pt::seconds(59));

    return pt::time_period(week_start, week_end);
}

std::string time::format_duration(const pt::time_duration &duration, bool brief) {
    std::stringstream ret;

    if (duration.hours() > 24) {
        ret << std::to_string((int) std::ceil(duration.hours() / 24)) + (brief ? "d" : " days ");
    }

    if (duration.hours() > 0) {
        ret << std::to_string(duration.hours() % 24) + (brief ? "h" : " hours ");
    }

    ret << std::to_string(duration.minutes()) + (brief ? "m" : " minutes");
    return ret.str();
}
