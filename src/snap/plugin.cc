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
#include "snap/lib_setup_impl.h"

#include <chrono>
#include <functional>
#include <sstream>
#include <string>

#include <grpc++/grpc++.h>

#include "snap/rpc/plugin.pb.h"

#include "snap/proxy/collector_proxy.h"
#include "snap/proxy/processor_proxy.h"
#include "snap/proxy/publisher_proxy.h"

using std::function;
using std::runtime_error;
using std::shared_ptr;
using std::unique_ptr;
using std::string;

using grpc::Server;
using grpc::ServerBuilder;

static void start_plugin(Plugin::PluginInterface* plugin, const Plugin::Meta& meta);

function<unique_ptr<Plugin::PluginExporter, function<void(Plugin::PluginExporter*)>>()> Plugin::LibSetup::exporter_provider = []{ return std::unique_ptr<PluginExporter>(new GRPCExporter()); };

Plugin::PluginException::PluginException(const string& message) :
                                         runtime_error(message) {}

Plugin::Meta::Meta(Type type, std::string name, int version) :
                     type(type),
                     name(name),
                     version(version),
                     rpc_type(RpcType::GRPC),
                     concurrency_count(5),
                     exclusive(false),
                     cache_ttl(std::chrono::milliseconds(500)),
                     strategy(Strategy::LRU) {}

Plugin::CollectorInterface* Plugin::PluginInterface::IsCollector() {
  return nullptr;
}

Plugin::ProcessorInterface* Plugin::PluginInterface::IsProcessor() {
  return nullptr;
}

Plugin::PublisherInterface* Plugin::PluginInterface::IsPublisher() {
  return nullptr;
}

Plugin::Type Plugin::CollectorInterface::GetType() const {
  return Collector;
}

Plugin::CollectorInterface* Plugin::CollectorInterface::IsCollector() {
  return this;
}

Plugin::Type Plugin::ProcessorInterface::GetType() const {
  return Processor;
}

Plugin::ProcessorInterface* Plugin::ProcessorInterface::IsProcessor() {
  return this;
}

Plugin::Type Plugin::PublisherInterface::GetType() const {
  return Publisher;
}

Plugin::PublisherInterface* Plugin::PublisherInterface::IsPublisher() {
  return this;
}

void Plugin::start_collector(CollectorInterface* collector,
                             const Meta& meta) {
  start_plugin(collector, meta);
}

void Plugin::start_processor(ProcessorInterface* processor,
                             const Meta& meta) {
  start_plugin(processor, meta);
}

void Plugin::start_publisher(PublisherInterface* publisher,
                             const Meta& meta) {
  start_plugin(publisher, meta);
}

static void start_plugin(Plugin::PluginInterface* plugin, const Plugin::Meta& meta) {
  auto exporter = Plugin::LibSetup::exporter_provider();
  // disable deleting the plugin instance
  auto plugin_ptr = shared_ptr<Plugin::PluginInterface>(plugin, [](void*){});
  auto completion = exporter->ExportPlugin(plugin_ptr, &meta);
  completion.get();
}
