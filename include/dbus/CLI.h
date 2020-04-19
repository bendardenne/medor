//
// Created by bdardenn on 4/14/20.
//

#ifndef MEDOR_CLI_H
#define MEDOR_CLI_H

#include <boost/date_time/posix_time/time_period.hpp>
#include <storage/DB.h>

namespace medor::dbus {
    namespace pt = boost::posix_time;

    class CLI {
    public:
        explicit CLI(const std::string& database_file);

        void start(const std::string &activity);

        void stop();

        void resume();

        void status(std::string format);

        void projects();

        void report(pt::time_period period);

    private:
        medor::storage::DB _database;
        std::unique_ptr<sdbus::IProxy> _trackerProxy;
    };
}


#endif //MEDOR_CLI_H
