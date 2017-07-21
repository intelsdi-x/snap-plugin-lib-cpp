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

#include <chrono>
#include <future>
#include <memory>
#include <string>
#include <vector>

#include <grpc++/grpc++.h>
#include <json.hpp>

#include "snap/config.h"
#include "snap/metric.h"

#define RPC_VERSION 1

namespace Plugin {
    /*
    * Implementation details for GRPC plugin exporter.
    *
    * Instance is supposed to be held referenced by shared pointer, to retain
    * the resources (ie.: service and server).
    */
    class GRPCExportImpl : public std::enable_shared_from_this<GRPCExportImpl> {
    public:
        std::future<void> DoExport(std::shared_ptr<PluginInterface> plugin, const Meta* meta);
        struct handler_type;

    protected:
        int port;
        std::shared_ptr<PluginInterface> plugin;
        const Meta* meta;
        std::unique_ptr<grpc::Service> service;
        std::unique_ptr<grpc::ServerBuilder> builder;
        std::unique_ptr<grpc::Server> server;

        /* steps of the export procedure */
        void doConfigure();
        void doRegister();
        nlohmann::json printPreamble();

        /* blocking method - waits for the server to finish. */
        void doJoin();

        int start_stand_alone(const int &httpPort);
    };
}
