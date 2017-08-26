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
#include <chrono>
#include <functional>
#include <sstream>
#include <string>
#include <thread>

#include <grpc++/grpc++.h>

#include "snap/plugin.h"
#include "snap/grpc_export.h"
#include "snap/lib_setup_impl.h"
#include "snap/rpc/plugin.pb.h"
#include "snap/proxy/collector_proxy.h"
#include "snap/proxy/processor_proxy.h"
#include "snap/proxy/publisher_proxy.h"
#include "snap/proxy/stream_collector_proxy.h"
#include "snap/flags.h"

using std::function;
using std::runtime_error;
using std::shared_ptr;
using std::unique_ptr;
using std::string;

using grpc::Server;
using grpc::ServerBuilder;

static void start_plugin(Plugin::PluginInterface* plugin, const Plugin::Meta& meta);
int start_stand_alone(Plugin::PluginInterface* plugin, const Plugin::Meta& meta);

function<unique_ptr<Plugin::PluginExporter, function<void(Plugin::PluginExporter*)>>()> Plugin::LibSetup::exporter_provider = []{ return std::unique_ptr<PluginExporter>(new GRPCExporter()); };

Plugin::PluginException::PluginException(const std::string& message) :
                                            runtime_error(message) {}

Plugin::Meta::Meta(Type type, std::string name, int version, RpcType rpc_type) :
                    type(type),
                    name(name),
                    version(version),
                    rpc_type(rpc_type),
                    concurrency_count(5),
                    exclusive(false),
                    unsecure(true),
                    cache_ttl(std::chrono::milliseconds(500)),
                    strategy(Strategy::LRU),
                    listen_port(""),
                    listen_addr("127.0.0.1"),
                    pprof_enabled(false),
                    tls_enabled(false),
                    tls_certificate_key_path(""),
                    tls_certificate_crt_path(""),
                    tls_certificate_authority_paths(""),
                    stand_alone(false),
                    stand_alone_port(stand_alone_port),
                    max_collect_duration(std::chrono::seconds(10)),
                    max_metrics_buffer(0) {}

void Plugin::Meta::use_cli_args(Flags *flags) {
	listen_port = flags->GetFlagStrValue("port");
	listen_addr = flags->GetFlagStrValue("addr");
	pprof_enabled = flags->IsParsedFlag("pprof");
	tls_enabled = flags->IsParsedFlag("tls");
	tls_certificate_crt_path = flags->GetFlagStrValue("cert-path");
	tls_certificate_key_path = flags->GetFlagStrValue("key-path");
	tls_certificate_authority_paths = flags->GetFlagStrValue("root-cert-paths");
	stand_alone = flags->IsParsedFlag("stand-alone");
	stand_alone_port = flags->GetFlagIntValue("stand-alone-port");
	max_collect_duration = std::chrono::seconds(flags->GetFlagIntValue("max-collect-duration"));
	max_metrics_buffer = flags->GetFlagInt64Value("max-metrics-buffer");
}

Plugin::CollectorInterface* Plugin::PluginInterface::IsCollector() {
    return nullptr;
}

Plugin::ProcessorInterface* Plugin::PluginInterface::IsProcessor() {
    return nullptr;
}

Plugin::PublisherInterface* Plugin::PluginInterface::IsPublisher() {
    return nullptr;
}

Plugin::StreamCollectorInterface* Plugin::PluginInterface::IsStreamCollector() {
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

Plugin::Type Plugin::StreamCollectorInterface::GetType() const {
    return StreamCollector;
}

Plugin::StreamCollectorInterface* Plugin::StreamCollectorInterface::IsStreamCollector() {
    return this;
}

void Plugin::start_collector(int argc, char **argv, CollectorInterface* collector,
                             Meta& meta) {
    Flags cli(argc, argv);
    meta.use_cli_args(&cli);

    if (cli.IsParsedFlag("version")) {
        cout << meta.name << " version "  << meta.version << endl;
        exit(0);
    }
    start_plugin(collector, meta);
}

void Plugin::start_processor(int argc, char **argv, ProcessorInterface* processor,
                             Meta& meta) {
    Flags cli(argc, argv);
    meta.use_cli_args(&cli);

    if (cli.IsParsedFlag("version")) {
        cout << meta.name << " version "  << meta.version << endl;
        exit(0);
    }

    start_plugin(processor, meta);
}

void Plugin::start_publisher(int argc, char **argv, PublisherInterface* publisher,
                             Meta& meta) {
    Flags cli(argc, argv);
    meta.use_cli_args(&cli);

    if (cli.IsParsedFlag("version")) {
        cout << meta.name << " version "  << meta.version << endl;
        exit(0);
    }

    start_plugin(publisher, meta);
}

void Plugin::start_stream_collector(int argc, char **argv, StreamCollectorInterface* stream_collector,
                             Meta& meta) {
    Flags cli(argc, argv);
    meta.use_cli_args(&cli);

    if (cli.IsParsedFlag("version")) {
        cout << meta.name << " version "  << meta.version << endl;
        exit(0);
    }

    stream_collector->SetMaxCollectDuration(meta.max_collect_duration);
    stream_collector->SetMaxMetricsBuffer(meta.max_metrics_buffer);

    start_plugin(stream_collector, meta);
}

static void start_plugin(Plugin::PluginInterface* plugin, const Plugin::Meta& meta) {
    auto exporter = Plugin::LibSetup::exporter_provider();
    // disable deleting the plugin instance
    auto plugin_ptr = shared_ptr<Plugin::PluginInterface>(plugin, [](void*){});
    auto completion = exporter->ExportPlugin(plugin_ptr, &meta);
    completion.get();
}
