//
// Created by bdardenn on 4/19/20.
//

#include "util/time.h"
#include "model/Activity.h"

// using namespace medor::util;

using namespace medor::util;
namespace pt = boost::posix_time;

pt::time_period time::weekFromNow(unsigned int offset) {
    // Generators that give the first Monday (Sunday) before (after) a given date.
    greg::first_day_of_the_week_before monday(greg::Monday);
    greg::first_day_of_the_week_after sunday(greg::Sunday);

    // Go back offset amount of weeks
    greg::date now = greg::day_clock::local_day() - greg::date_duration(7 * offset);

    pt::ptime weekStart = pt::ptime(monday.get_date(now), pt::hours(0));

    // first_day_of_the_week_before will give us the previous Monday, if now is a Monday. We don't want that.
    if(now.day_of_week() == greg::Monday) {
        weekStart = pt::ptime(now, pt::hours(0));
    }

    pt::ptime weekEnd = pt::ptime(sunday.get_date(now), pt::hours(23) + pt::minutes(59) + pt::seconds(59));
    if(now.day_of_week() == greg::Sunday) {
        weekEnd = pt::ptime(now, pt::hours(23) + pt::minutes(59) + pt::seconds(59));
    }

    return pt::time_period(weekStart, weekEnd);
}

std::string time::formatDuration(const pt::time_duration& duration, bool brief) {
    std::stringstream ret;

    if (duration.hours() > 24) {
        ret << std::to_string((int)std::ceil(duration.hours() / 24)) + (brief ? "d" : " days ");
    }

    if (duration.hours() > 0) {
        ret << std::to_string(duration.hours() % 24) + (brief ? "h" : " hours ");
    }

    ret << std::to_string(duration.minutes()) + (brief ? "m" : " minutes");
    return ret.str();
}

pt::time_duration time::aggregateTimes(const std::vector<model::Activity>& activities) {
    pt::time_duration total;

    for (const auto& activity : activities) {
        //TODO consider exposing a time_period on the Activity?
        pt::time_period period(activity.getStart(), activity.getEnd());
        total += period.length();
    }

    return total;
}