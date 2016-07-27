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
#include "snap/plugin.h"

#include <chrono>
#include <iostream>
#include <sstream>
#include <string>

#include <grpc++/grpc++.h>

#include <json.hpp>

#include "snap/rpc/plugin.pb.h"

#include "snap/proxy/collector_proxy.h"
#include "snap/proxy/processor_proxy.h"
#include "snap/proxy/publisher_proxy.h"

using std::cout;
using std::endl;

using grpc::Server;
using grpc::ServerBuilder;

using json = nlohmann::json;

static void emit_preamble(const Plugin::Meta& meta, int port);

Plugin::Meta::Meta(Type type, std::string name, int version) :
                     type(type),
                     name(name),
                     version(version),
                     rpc_type(RpcType::GRPC),
                     concurrency_count(5),
                     exclusive(false),
                     unsecure(false),
                     cache_ttl(std::chrono::milliseconds(500)),
                     strategy(Strategy::LRU) {}

void Plugin::start_collector(CollectorInterface* collector,
                             const Meta& meta) {
    std::string server_address = "127.0.0.1:0";
    int port;

    Proxy::CollectorImpl collector_impl(collector);

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials(),
                             &port);
    builder.RegisterService(&collector_impl);
    std::unique_ptr<Server> server(builder.BuildAndStart());

    emit_preamble(meta, port);

    server->Wait();
}

void Plugin::start_processor(ProcessorInterface* processor,
                             const Meta& meta) {
    std::string server_address = "127.0.0.1:0";
    int port;

    Proxy::ProcessorImpl processor_impl(processor);

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials(),
                             &port);
    builder.RegisterService(&processor_impl);
    std::unique_ptr<Server> server(builder.BuildAndStart());

    emit_preamble(meta, port);

    server->Wait();
}

void Plugin::start_publisher(PublisherInterface* publisher,
                             const Meta& meta) {
    std::string server_address = "127.0.0.1:0";
    int port;

    Proxy::PublisherImpl publisher_impl(publisher);

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials(),
                             &port);
    builder.RegisterService(&publisher_impl);
    std::unique_ptr<Server> server(builder.BuildAndStart());

    emit_preamble(meta, port);

    server->Wait();
}

static void emit_preamble(const Plugin::Meta& meta, int port) {
  std::stringstream ss;
  ss << "127.0.0.1:" << port;
  json j = {
    {"Meta", {
      {"Type", meta.type},
      {"Name", meta.name},
      {"Version", meta.version},
      {"RPCType", meta.rpc_type},
      {"RPCVersion", RPC_VERSION},
      {"ConcurrencyCount", meta.concurrency_count},
      {"Exclusive", meta.exclusive},
      {"Unsecure", meta.unsecure},
      {"CacheTTL", meta.cache_ttl.count()},
      {"RoutingStrategy", meta.strategy}
    }},
    {"ListenAddress", ss.str()},
    {"Type", meta.type},
    {"State", 0},
    {"ErrMessage", ""},
    {"Version", meta.version},
  };
  cout << j << endl;
}
