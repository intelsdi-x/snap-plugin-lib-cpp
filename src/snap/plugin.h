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

#include "snap/config.h"
#include "snap/metric.h"
#include "snap/flags.h"

#define RPC_VERSION 1

namespace Plugin {

    class CollectorInterface;
    class ProcessorInterface;
    class PublisherInterface;

    /**
    * Type is the plugin type
    */
    enum Type {
        Collector,
        Processor,
        Publisher
    };

    /**
    * RpcType is the rpc protocol the plugin should use.
    * Only GRPC is supported for C++.
    */
    enum RpcType {
        GRPC = 2,
        GRPCStream = 3
    };

    /**
    * Strategy is the routing and caching strategy this plugin requires.
    * A routing and caching Strategy is the method which snapteld uses to cache
    * metrics, and select the correct running instance of a plugin.
    */
    enum Strategy {
        /**
        * LRU: Least Recently Used.
        * The default strategy.
        */
        LRU,

        /**
        * Sticky: Sticky Running plugins have a 1:1 relationship to Tasks.
        */
        Sticky,

        /** ConfigBased: Bases routing decisions on the incoming config.
        * The config based strategy hashes the incoming config data and
        * uses it as a key to select a running plugin.  It can be useful for plugins
        * which maintain a connection to a database, for instance.
        */
        ConfigBased
    };

    /**
    * Meta is the metadata about the plugin.
    */
    struct Meta final {
    public:
        Meta(Type type, std::string name, int version);
        Meta(Type type, std::string name, int version, Flags *flags);

        Type type;
        std::string name;
        int version;

        /**
        * The RpcType in use, defaults to RpcType::GRPC. There should be no need to change
        * it.
        */
        RpcType rpc_type;

        /**
        * concurrency_count is the max number of concurrent calls the plugin
        * should take.  For example:
        * If there are 5 tasks using the plugin and its concurrency count is 2,
        * snapteld will keep 3 plugin instances running.
        * Using concurrency_count overwrites the default value of (5).
        */
        int concurrency_count;

        /**
        * exclusive == true results in a single instance of the plugin running
        * regardless of the number of tasks using the plugin.
        * Using exclusive overwrites the default value of (false).
        */
        bool exclusive;

        /**
        * Unsecure is a legacy value not used for grpc, but needed to avoid
		* calling SetKey needlessly.
        */
        bool unsecure;

        /**
        * snapteld caches metrics on the daemon side for a default of 500ms.
        * CacheTTL overwrites the default value of (500ms).
        */
        std::chrono::milliseconds cache_ttl;

        /**
        * Strategy will override the routing strategy this plugin requires.
        * The default routing strategy is Least Recently Used.
        * Strategy overwrites the default value of (LRU).
        */
        Strategy strategy;

        /**
        * Port GRPC will listen on
        */
        std::string listen_port;

        /**
        * Address GRPC will listen on
        */
        std::string listen_addr;        

        /**
        * Enables pprof
        */
        bool pprof_enabled;

        /**
        * Enables TLS
        */
        bool tls_enabled;

        /**
        * Necessary to provide when TLS enabled
        */
        std::string cert_path;

        /**
        * Necessary to provide when TLS enabled
        */
        std::string key_path;

        /**
        * Root path separator
        */
        std::string root_cert_paths;

        /**
        * Enable stand-alone plugin
        */
        bool stand_alone;

        /**
        * Specify http port when stand-alone plugin is enabled
        */
        int stand_alone_port;

        /**
        * sets the maximum duration (always greater than 0s) between collections
        * before metrics are sent. Defaults to 10s what means that after 10 seconds 
        * no new metrics are received, the plugin should send whatever data it has
        * in the buffer instead of waiting longer. (e.g. 5s)
        */
        std::chrono::seconds max_collect_duration;

        /**
        * maximum number of metrics the plugin is buffering before sending metrics.
        * Defaults to zero what means send metrics immediately
        */
        int64_t max_metrics_buffer;
    };

    /**
    * Base class for exceptions indicated by plugins.
    *
    * When thrown from any kind of plugin, the exception will be
    * propagated to and reported by Snap framework.
    */
    class PluginException : public std::runtime_error {
    public:
        PluginException(const std::string& message);
    };

    /**
    * PluginInterface is the interface implemented by ALL plugins.
    * Every plugin must implement get_config_policy.
    */
    class PluginInterface {
    public:
        virtual ~PluginInterface() {}
        virtual Type GetType() const = 0;
        virtual CollectorInterface* IsCollector();
        virtual ProcessorInterface* IsProcessor();
        virtual PublisherInterface* IsPublisher();

        virtual const ConfigPolicy get_config_policy() = 0;
    protected:
        PluginInterface() = default;
    };

    /**
    * PluginExporter is the driver for exporting plugin as a service (e.g.: via
    * GRPC). It's supposed to export a single instance of a plugin - should not be
    * reused.
    *
    * PluginExporter can block waiting for plugin termination. Any resources
    * associated with export operation are retained until the blocking wait
    * completes or exporter instance is destroyed.
    *
    * Plugin Library offers default plugin export implementation based on GRPC, if
    * the static start_xxx methods are used.
    */
    class PluginExporter {
    public:
        virtual ~PluginExporter() = default;

        PluginExporter(const PluginExporter &) = delete;
        PluginExporter& operator=(const PluginExporter &) = delete;

        /** Export plugin as a service, allow waiting for termination (via future). */
        virtual std::future<void> ExportPlugin(std::shared_ptr<PluginInterface> plugin, const Meta* meta) = 0;
    protected:
        PluginExporter() = default;
    };

    /**
    * The interface for a collector plugin.
    * A Collector is the source.
    * It is responsible for collecting metrics in the Snap pipeline.
    */
    class CollectorInterface : public PluginInterface {
    public:
        Type GetType() const final;
        CollectorInterface* IsCollector() final;

        /*
        * (inherited from PluginInterface)
        */
        virtual const ConfigPolicy get_config_policy() = 0;

        /*
        * get_metric_types should report all the metrics this plugin can collect.
        */
        virtual std::vector<Metric> get_metric_types(Config cfg) = 0;

        /*
        * collect_metrics is given a list of metrics to collect.
        * It should collect and annotate each metric with the apropos context.
        */
        virtual std::vector<Metric> collect_metrics(std::vector<Metric> &metrics) = 0;
    };

    /**
    * The interface for a Processor plugin.
    * A Processor is an intermediary in the pipeline. It may decorate, filter, or
    * derive as data passes through Snap's pipeline.
    */
    class ProcessorInterface : public PluginInterface {
    public:
        Type GetType() const final;
        ProcessorInterface* IsProcessor() final;

        virtual void process_metrics(std::vector<Metric> &metrics,
                                    const Config& config) = 0;
    };

    /**
    * The interface for a Publisher plugin.
    * A Publisher is the sink.
    * It sinks data into some external system.
    */
    class PublisherInterface : public PluginInterface {
    public:
        Type GetType() const final;
        PublisherInterface* IsPublisher() final;

        virtual void publish_metrics(std::vector<Metric> &metrics,
                                    const Config& config) = 0;
    };

    /**
    * These functions are called to start a plugin.
    * They export plugin using default PluginExporter based on GRPC:
    * construct the gRPC service and server, start them, and then
    * block forever.
    *
    * These functions do not manage the plugin instance passed as parameter -
    * caller's responsible for releasing the resources.
    */
    void start_collector(CollectorInterface* plg, const Meta& meta);
    void start_processor(ProcessorInterface* plg, const Meta& meta);
    void start_publisher(PublisherInterface* plg, const Meta& meta);

};  // namespace Plugin
