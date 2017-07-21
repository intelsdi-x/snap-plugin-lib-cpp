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
#include <grpc++/grpc++.h>
#include <iostream>
#include <thread>

#include "snap/proxy/plugin_proxy.h"
#include "snap/rpc/plugin.pb.h"

using grpc::Server;
using grpc::ServerContext;
using grpc::Status;

using rpc::Empty;
using rpc::ErrReply;
using rpc::KillArg;
using rpc::GetConfigPolicyReply;

using Plugin::Proxy::PluginImpl;

PluginImpl::PluginImpl(Plugin::PluginInterface* plugin) : plugin(plugin) {}

Status PluginImpl::Ping(ServerContext* context, const Empty* req,
                        ErrReply* resp) {
    _lastPing = std::chrono::system_clock::now();
    // Change to log
    std::cout << "Heartbeat received at: " << _lastPing << std::endl;
    return Status::OK;
}

Status PluginImpl::Kill(ServerContext* context, const KillArg* req,
                        ErrReply* resp) {
    return Status::OK;
}

Status PluginImpl::GetConfigPolicy(ServerContext* context, const Empty* req,
                                   GetConfigPolicyReply* resp) {
    *resp = plugin->get_config_policy();
    return Status::OK;
}

void PluginImpl::HeartbeatWatch() {
    _lastPing = std::chrono::system_clock::now();
    std::cout << "Heartbeat started" << std::endl;
    int count = 0;
    while (1) {
        if ((std::chrono::system_clock::now() - _lastPing).count() >= _pingTimeoutDuration.count()) {
            ++count;
            std::cout << "Heartbeat timeout " << count 
                        << " of " << _pingTimeoutLimit 
                        << ".  (Duration between checks " << _pingTimeoutDuration.count() << ")" << std::endl;
            if (count >= _pingTimeoutLimit) {
                std::cout << "Heartbeat timeout expired!" << std::endl;
                exit(0);
            }
        } else {
            std::cout << "Heartbeat timeout reset";
            count = 0;
        }
        std::this_thread::sleep_for(_pingTimeoutDuration);
    }
}
