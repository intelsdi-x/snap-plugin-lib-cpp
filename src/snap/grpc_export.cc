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
#include "snap/grpc_export.h"
#include "snap/grpc_export_impl.h"

#include <chrono>
#include <future>
#include <iostream>
#include <memory>
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
using std::future;
using std::runtime_error;
using std::string;
using std::shared_ptr;
using std::unique_ptr;

using grpc::Server;
using grpc::ServerBuilder;

using json = nlohmann::json;

using Plugin::PluginInterface;
using Plugin::Meta;

Plugin::GRPCExporter::GRPCExporter() : impl(std::shared_ptr<GRPCExportImpl>(implement())) {}

future<void> Plugin::GRPCExporter::ExportPlugin(shared_ptr<PluginInterface> plugin, const Meta* meta) {
  return this->impl->DoExport(plugin, meta);
}

Plugin::GRPCExportImpl* Plugin::GRPCExporter::implement() {
  return new GRPCExportImpl();
}

future<void> Plugin::GRPCExportImpl::DoExport(shared_ptr<PluginInterface> plugin, const Meta *meta) {
  this->plugin = std::move(plugin);
  this->meta = meta;
  doConfigure();
  doRegister();
  doAdvertise();
  auto self = this->shared_from_this();
  return std::async(std::launch::deferred, [=](){ self->doJoin(); });
}

void Plugin::GRPCExportImpl::doConfigure() {
  string server_address = "127.0.0.1:0";

  switch (plugin->GetType()) {
    case Plugin::Collector:
      this->service.reset(new Proxy::CollectorImpl(plugin->IsCollector()));
      break;
    case Plugin::Processor:
      this->service.reset(new Proxy::ProcessorImpl(plugin->IsProcessor()));
      break;
    case Plugin::Publisher:
      this->service.reset(new Proxy::PublisherImpl(plugin->IsPublisher()));
      break;
  }
  builder.reset(new grpc::ServerBuilder());
  builder->AddListeningPort(server_address, grpc::InsecureServerCredentials(),
                           &this->port);

}

void Plugin::GRPCExportImpl::doRegister() {
  builder->RegisterService(service.get());
  this->server = std::move(builder->BuildAndStart());
}

void Plugin::GRPCExportImpl::doAdvertise() {
  std::stringstream ss;
  ss << "127.0.0.1:" << port;
  json j = {
      {"Meta", {
                   {"Type", meta->type},
                   {"Name", meta->name},
                   {"Version", meta->version},
                   {"RPCType", meta->rpc_type},
                   {"RPCVersion", RPC_VERSION},
                   {"ConcurrencyCount", meta->concurrency_count},
                   {"Exclusive", meta->exclusive},

                   // The gRPC client in Snap does not use the `Unsecure` metadata key at
                   // this time, as it is used for payload encryption.  With gRPC, encryption
                   // is done via its transport, and this will be updated once support for
                   // that feature lands in snapteld.
                   {"Unsecure", true},
                   {"CacheTTL", meta->cache_ttl.count()},
                   {"RoutingStrategy", meta->strategy}
               }},
      {"ListenAddress", ss.str()},
      {"Type", meta->type},
      {"State", 0},
      {"ErrMessage", ""},
      {"Version", meta->version},
  };
  cout << j << endl;
}

void Plugin::GRPCExportImpl::doJoin() {
  server->Wait();
}
