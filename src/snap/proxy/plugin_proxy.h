/*
http://www.apache.org/licenses/LICENSE-2.0.txt
Copyright 2016 Intel Corporation
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/
#pragma once
#include <iostream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <grpc++/grpc++.h>

#include "snap/rpc/plugin.pb.h"
#include "snap/plugin.h"

namespace Plugin {
    namespace Proxy {
        template<typename Clock, typename Duration>
        std::ostream &operator<<(std::ostream &stream,
        const std::chrono::time_point<Clock, Duration> &time_point) {
            const time_t time = Clock::to_time_t(time_point);
        #if __GNUC__ > 4 || \
            ((__GNUC__ == 4) && __GNUC_MINOR__ > 8 && __GNUC_REVISION__ > 1)
            // Maybe the put_time will be implemented later?
            struct tm tm;
            localtime_r(&time, &tm);
            return stream << std::put_time(&tm, "%c"); // Print standard date&time
        #else
            char buffer[26];
            ctime_r(&time, buffer);
            buffer[24] = '\0';  // Removes the newline that is added
            return stream << buffer;
        #endif
        }

        class PluginImpl final {
        public:
            explicit PluginImpl(Plugin::PluginInterface* plugin);

            grpc::Status Ping(grpc::ServerContext* context, const rpc::Empty* req,
                                rpc::ErrReply* resp);


            grpc::Status Kill(grpc::ServerContext* context, const rpc::KillArg* req,
                                rpc::ErrReply* response);

            grpc::Status GetConfigPolicy(grpc::ServerContext* context,
                                        const rpc::Empty* req,
                                        rpc::GetConfigPolicyReply* resp);

            void HeartbeatWatch();

        private:
            Plugin::PluginInterface* plugin;
            std::chrono::system_clock::time_point _lastPing;
            // PingTimeoutLimit is the number of successively missed pin health checks
            // which must occur before the plugin is stopped
            int _pingTimeoutLimit = 3;            
            // PingTimeoutDuration is the duration during which a ping healthcheck
            // should be received
            std::chrono::seconds _pingTimeoutDuration{3}; 
        };
    }  // namespace Proxy
}  // namespace Plugin
