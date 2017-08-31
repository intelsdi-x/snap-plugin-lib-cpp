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
#include "snap/flags.h"

using std::function;
using std::runtime_error;
using std::shared_ptr;
using std::unique_ptr;
using std::string;
using std::cout;

using grpc::Server;
using grpc::ServerBuilder;

static void start_plugin(Plugin::PluginInterface* plugin, const Plugin::Meta& meta);
int start_stand_alone(Plugin::PluginInterface* plugin, const Plugin::Meta& meta);

function<unique_ptr<Plugin::PluginExporter, function<void(Plugin::PluginExporter*)>>()> Plugin::LibSetup::exporter_provider = []{ return std::unique_ptr<PluginExporter>(new GRPCExporter()); };

Plugin::PluginException::PluginException(const std::string& message) :
                                            runtime_error(message) {}


Plugin::Meta::Meta(Type type, std::string name, int version) :
                    type(type),
                    name(name),
                    version(version),
                    rpc_type(RpcType::GRPC),
                    rpc_version(1),
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
                    diagnostic_enabled(false),
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
    diagnostic_enabled = !stand_alone && !flags->IsConfigFromFramework();
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

void Plugin::start_collector(int argc, char **argv, CollectorInterface* collector,
                             Meta& meta) {
    Flags cli(argc, argv);
    meta.use_cli_args(&cli);

    if (cli.IsParsedFlag("version")) {
        cout << meta.name << " version "  << meta.version << endl;
        exit(0);
    }
    if (! meta.diagnostic_enabled) {
        start_plugin(collector, meta);
    } else {
        DiagnosticPrinter diagnostics(collector, meta, cli);
        diagnostics.show();
    }
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

static void start_plugin(Plugin::PluginInterface* plugin, const Plugin::Meta& meta) {
    auto exporter = Plugin::LibSetup::exporter_provider();
    // disable deleting the plugin instance
    auto plugin_ptr = shared_ptr<Plugin::PluginInterface>(plugin, [](void*){});
    auto completion = exporter->ExportPlugin(plugin_ptr, &meta);
    completion.get();
}

Plugin::DiagnosticPrinter::DiagnosticPrinter(CollectorInterface* collector, const Meta& meta, Flags& cli, std::ostream& os) :
                                                                                                   os(os),
                                                                                                   collector(collector),
                                                                                                   meta(meta),
                                                                                                   cfgmap(cli.GenerateConfigMapFromCommandJson()),
                                                                                                   config(cfgmap) {}

Plugin::DiagnosticPrinter::~DiagnosticPrinter() {}

void Plugin::DiagnosticPrinter::show() {
    Stopwatch diagnostic_watch(os);
    diagnostic_watch.start();

    print_runtime_details();
    print_config_policy();
    std::vector<Metric> mts = print_metric_types();
    print_collect_metrics(mts);
    print_contact_us();

    diagnostic_watch.print_elapsed("show_diagnostics took ","\n\n");
}

void Plugin::DiagnosticPrinter::print_contact_us() {
    os << "\nThank you for using this Snap plugin. If you have questions or are running\n" 
            "into errors, please contact us on Github (github.com/intelsdi-x/snap) or\n" 
            "our Slack channel (intelsdi-x.herokuapp.com).\n" 
            "The repo for this plugin can be found: github.com/intelsdi-x/<plugin-name>.\n" 
            "When submitting a new issue on Github, please include this diagnostic\n" 
            "print out so that we have a starting point for addressing your question.\n" 
            "Thank you.\n\n";
}

void Plugin::DiagnosticPrinter::print_runtime_details() {
    Stopwatch timer(os);
    std::vector<std::string> RPCTypeString {"" ,"" ,"GRPC" ,"GRPCStream"};
    timer.start();

    os << "Runtime Details:\n"
            "    PluginName: " << meta.name << ", Version: " << meta.version << "\n"
            "    RPC Type: " << RPCTypeString[meta.rpc_type] << ", RPC Version: " << meta.rpc_version << "\n"
            "    Operating system: " << get_os_name() << "\n"
            "    Architecture: " << get_architecture_name() << "\n"
            "    gcc version: " << __GNUC__ << "." << __GNUC_MINOR__ << "." << __GNUC_PATCHLEVEL__ << "\n";
    timer.print_elapsed("print_runtime_details took ","\n");
}

void Plugin::DiagnosticPrinter::print_config_policy() {
    Stopwatch timer(os);
    timer.start();

    ConfigPolicy cpolicy = collector->get_config_policy();
    config.apply_defaults(cpolicy);

    os << "\nConfig Policy:\n";
    os << std::resetiosflags(std::ios::adjustfield);
    os << std::setiosflags(std::ios::left);
    os  << setw(40) << "NAMESPACE"  << setw(25) << "KEY"  << setw(20) << "TYPE"<< setw(20) << "REQUIRED"<< setw(20) << "DEFAULT"<< setw(20) << "MINIMUM" << setw(20) << "MAXIMUM" << "\n";
    print_string_policy(cpolicy);
    print_integer_policy(cpolicy);
    print_bool_policy(cpolicy);

    timer.print_elapsed("print_config_policy took ","\n");
}

std::vector<Plugin::Metric> Plugin::DiagnosticPrinter::print_metric_types() {
    Stopwatch timer(os);
    timer.start();
    std::vector<Metric> metrics = collector->get_metric_types(config);

    os << "\nMetric catalog will be updated to include following namespaces:\n";
    for (auto& metric : metrics) {
        os << "    Namespace: " << metric.ns().get_string() << "\n";
    }
    timer.print_elapsed("print_metric_types took ","\n");
    return metrics;
}

void Plugin::DiagnosticPrinter::print_collect_metrics(std::vector<Metric> metric_types) {
    Stopwatch timer(os);
    timer.start();

    for (auto& metric : metric_types) {
        metric.set_diagnostic_config(config);
    }
    os << "\nMetrics that can be collected right now are:\n";
    std::vector<Metric> mts = collector->collect_metrics(metric_types);
    for (auto& metric : mts) {
        os  << "    Namespace: " << setw(40) << metric.ns().get_string() << setw(6) << "Type: " << setw(20) << metric.data_type() << setw(8) << " Value: ";
        switch (metric.data_type()){
            case Metric::Int32 : os << metric.get_int_data() << "\n";
                break;
            case Metric::Uint32 : os << metric.get_uint32_data() << "\n";
                break;
            case Metric::String : os << metric.get_string_data() << "\n";
                break;
            case Metric::Bool : os << metric.get_bool_data() << "\n";
                break;
            case Metric::Float32 : os << metric.get_float32_data() << "\n";
                break;
            case Metric::Float64 : os << metric.get_float64_data() << "\n";
                break;
            case Metric::Int64 : os << metric.get_int64_data() << "\n";
                break;
            case Metric::Uint64 : os << metric.get_uint64_data() << "\n";
                break;
        }
    }
    timer.print_elapsed("print_collect_metrics took ","\n");
}

Plugin::DiagnosticPrinter::Stopwatch::Stopwatch(std::ostream& os ) :
                        os(os),
                        started(false),
                        stopped(false),
                        unit("µs") {}

Plugin::DiagnosticPrinter::Stopwatch::~Stopwatch() {}

void Plugin::DiagnosticPrinter::Stopwatch::start() {
    begin = std::chrono::high_resolution_clock::now();
    started = true;
}

void Plugin::DiagnosticPrinter::Stopwatch::stop() {
    end = std::chrono::high_resolution_clock::now();
    stopped = true;
}

void Plugin::DiagnosticPrinter::Stopwatch::print_elapsed(std::string message_before,
                                      std::string message_after) {
    if (!started) {
        std::cerr<<"Stopwatch must be started first!\n";
        this->start();
    }
    if (!stopped) {
        this->stop();
    }
    auto result = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
    if ( result > 1000.00) {
        result /= 1000.00;
        unit = "ms";
    }
    if ( result > 1000.00) {
        result /= 1000.00;
        unit = "s";
    }
    os << message_before << result << unit << message_after;
}

std::string Plugin::DiagnosticPrinter::get_os_name()
{
    #ifdef __linux__
    return "linux";
    #elif _WIN32
    return "windows 32-bit";
    #elif _WIN64
    return "windows 64-bit";
    #elif __APPLE__ || __MACH__
    return "mac OSX";
    #else
    return "unknown";
    #endif
}

std::string Plugin::DiagnosticPrinter::get_architecture_name(){
    #ifdef __i386
    return "Intel x86";
    #elif __amd64__
    return "amd64";
    #elif __ia64__
    return "Intel Itanium";
    #else
    return "unknown";
    #endif
}

void Plugin::DiagnosticPrinter::print_string_policy(ConfigPolicy& cpolicy){
    for(auto& str_policy : cpolicy.string_policy()) {
        for (auto& rule : str_policy.second.rules()) {
            std::string default_ = (rule.second.has_default()) ? rule.second.default_() : "";
            os << setw(40) << str_policy.first
               << setw(25) << rule.first 
               << setw(20) << "string" 
               << setw(20) << std::boolalpha << rule.second.required()
               << setw(20) << default_
               << setw(20) << "" 
               << setw(20) << "" <<"\n";
        }
    }
}

void Plugin::DiagnosticPrinter::print_integer_policy(ConfigPolicy& cpolicy){
    for(auto& int_policy : cpolicy.integer_policy()) {
        for (auto& rule : int_policy.second.rules()) {
            std::string default_ = (rule.second.has_default()) ? std::to_string(rule.second.default_()) : "";
            std::string minimum_ = (rule.second.has_min()) ? std::to_string(rule.second.minimum()) : "";
            std::string maximum_ = (rule.second.has_max()) ? std::to_string(rule.second.maximum()) : "";
            os << setw(40) << int_policy.first
               << setw(25) << rule.first
               << setw(20) << "integer"
               << setw(20) << std::boolalpha << rule.second.required()
               << setw(20) << default_
               << setw(20) << minimum_
               << setw(20) << maximum_ <<"\n";
        }
    }
}

void Plugin::DiagnosticPrinter::print_bool_policy(ConfigPolicy& cpolicy){
    for(auto& bool_policy : cpolicy.bool_policy()) {
        for (auto& rule : bool_policy.second.rules()) {
            std::string default_ = (rule.second.has_default()) ? std::to_string(rule.second.default_()) : "";
            default_ = (default_ != "" && default_ == "1") ? "true" : "false";
            os << setw(40) << bool_policy.first
               << setw(25) << rule.first 
               << setw(20) << "bool" 
               << setw(20) << std::boolalpha << rule.second.required()
               << setw(20) << std::boolalpha << default_
               << setw(20) << "" 
               << setw(20) << "" <<"\n";
        }
    }
}
