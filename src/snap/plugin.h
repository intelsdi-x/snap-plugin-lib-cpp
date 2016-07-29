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
#include <string>
#include <vector>

#include "snap/config.h"
#include "snap/metric.h"

#define RPC_VERSION 1

namespace Plugin {

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
};

/**
 * Strategy is the routing and caching strategy this plugin requires.
 * A routing and caching Strategy is the method which snapd uses to cache 
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

  Type type;
  std::string name;
  int version;

  // These members all have defaults.
  RpcType rpc_type;                     // RpcType::GRPC
  int concurrency_count;                // 5
  bool exclusive;                       // false
  bool unsecure;                        // false
  std::chrono::milliseconds cache_ttl;  // 500ms
  Strategy strategy;                    // Strategy::LRU
};

/**
 * PluginInterface is the interface implemented by ALL plugins.
 * Every plugin must implement get_config_policy.
 */
class PluginInterface {
 public:
  virtual ~PluginInterface() {}
  virtual const ConfigPolicy get_config_policy() = 0;
};

/**
 * The interface for a collector plugin.
 * A Collector is the source.
 * It is responsible for collecting metrics in the snap pipeline.
 */
class CollectorInterface : public PluginInterface {
 public:
  virtual ~CollectorInterface() {}
  /*
   * get_metric_types should report all the metrics this plugin can collect.
   */
  virtual std::vector<Metric> get_metric_types(Config cfg) = 0;

  /*
   * collect_metrics is given a list of metrics to collect.
   * It should collect and annotate each metric with the apropos context.
   */
  virtual void collect_metrics(std::vector<Metric>* metrics) = 0;
};

/**
 * The interface for a Processor plugin.
 * A Processor is an intermediary in the pipeline. It may decorate, filter, or
 * derive as data passes through Snap's pipeline.
 */
class ProcessorInterface : public PluginInterface {
 public:
  virtual ~ProcessorInterface() {}
  virtual void process_metrics(std::vector<Metric>* metrics,
                               const Config& config) = 0;
};

/**
 * The interface for a Publisher plugin.
 * A Publisher is the sink.
 * It sinks data into some external system.
 */
class PublisherInterface : public PluginInterface {
 public:
  virtual ~PublisherInterface() {}
  virtual void publish_metrics(std::vector<Metric>* metrics,
                               const Config& config) = 0;
};

/**
 * These functions are called to start a plugin.
 * They construct the gRPC service and server, start them, and then
 * block forever.
 */
void start_collector(CollectorInterface* plg, const Meta& meta);
void start_processor(ProcessorInterface* plg, const Meta& meta);
void start_publisher(PublisherInterface* plg, const Meta& meta);

};  // namespace Plugin
