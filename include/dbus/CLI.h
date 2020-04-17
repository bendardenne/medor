//
// Created by bdardenn on 4/14/20.
//

#ifndef MEDOR_CLI_H
#define MEDOR_CLI_H

#include <boost/date_time/posix_time/posix_time_config.hpp>

namespace medor::dbus {
    class CLI {
    public:
        CLI();

        void start(const std::string& activity);

        void stop();

        void resume();

        void status(std::string format);

        void projects();
    private:

        static std::string format_duration(boost::posix_time::time_duration duration);

        std::unique_ptr<sdbus::IProxy> trackerProxy;
    };
}


#endif //MEDOR_CLI_H
