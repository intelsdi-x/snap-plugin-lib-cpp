<!--
http://www.apache.org/licenses/LICENSE-2.0.txt


Copyright 2017 Intel Corporation

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
-->

## Snap Plugin Library for C++: Streaming Collector Plugin Example
Here you will find an example plugin that covers the basics for writing a streaming collector plugin.

## Plugin Naming, Files, and Directory
For your streaming collector plugin, create a new repository and name your plugin project using the following format:

>snap-plugin-[plugin-type]-[plugin-name]

For example:
>snap-plugin-streaming-collector-rando

Proposed files and directory structure:  
```
snap-plugin-[plugin-type]-[plugin-name]
 |--src
  |--[plugin-name].cc  
  |--[plugin-name]_test.cc  
```

For example:
```
snap-plugin-streaming-collector-rando
 |--src
  |--rando.cc  
  |--rando_test.cc  
```

## Interface methods

In order to write a plugin for Snap, it is necessary to define a few methods to satisfy the appropriate interfaces. 
These interfaces must be defined for a streaming collector plugin:

```cpp
    /**
    * The interface for a stream collector plugin.
    * A Stream Collector is the source.
    * It is responsible for streaming metrics in the Snap pipeline.
    */
    class StreamCollectorInterface : public PluginInterface {
    public:
        Type GetType() const final;
        StreamCollectorInterface* IsStreamCollector() final;

        void SetMaxCollectDuration(std::chrono::seconds maxCollectDuration) {
            _max_collect_duration = maxCollectDuration;
        }
        void SetMaxCollectDuration(int64_t maxCollectDuration) {
            _max_collect_duration = std::chrono::seconds(maxCollectDuration);
        }
        std::chrono::seconds GetMaxCollectDuration() {
            return _max_collect_duration;
        }

        void SetMaxMetricsBuffer(int64_t maxMetricsBuffer) {
            _max_metrics_buffer = maxMetricsBuffer;
        }
        int64_t GetMaxMetricsBuffer() {
            return _max_metrics_buffer;
        }

        /*
        * (inherited from PluginInterface)
        */
        virtual const ConfigPolicy get_config_policy() = 0;

        virtual std::vector<Metric> get_metric_types(Config cfg) = 0;

        /* StreamMetrics allows the plugin to send/receive metrics on a channel
        * Arguments are (in order):
        *
        * A channel for metrics into the plugin from Snap -- which
        * are the metric types snap is requesting the plugin to collect.
        *
        * A channel for metrics from the plugin to Snap -- the actual
        * collected metrics from the plugin.
        *  
        * A channel for error strings that the library will report to snap
        * as task errors.
        */
        virtual void stream_metrics() = 0;

        virtual std::vector<Plugin::Metric> put_metrics_out() = 0;
        virtual std::string put_err_msg() = 0;
        virtual void get_metrics_in(std::vector<Plugin::Metric> &metsIn) = 0;
        virtual bool put_mets() = 0;
        virtual bool put_err() = 0;
        virtual void set_put_mets(const bool &putMets) = 0;
        virtual void set_put_err(const bool &putErr) = 0;
        virtual void set_context_cancelled(const bool &contextCancelled) = 0;
        virtual bool context_cancelled() = 0;
};
```

The interface is slightly different depending on what type (collector, processor, publisher, or streamin-collector) of plugin is being written.
Please see other plugin types for more details.

## Starting a plugin

After implementing a type that satisfies one of {collector, processor, publisher, stream-collector} interfaces,
all that is left to do is to call the appropriate plugin.start_xxx() with your plugin specific meta options.
For example with minimum meta data specified:

```cpp
    Plugin::start_stream_collector(&plg, Meta{Type::StreamCollector, "rando", 1, &cli, RpcType::GRPCStream});
```

### Meta options

The available options are defined in [src/snap/plugin.h](https://github.com/intelsdi-x/snap-plugin-lib-cpp/tree/master/src/snap/plugin.h).
This structure defines default values.
 
```cpp
  /**
    * Meta is the metadata about the plugin.
    */
    struct Meta final {
    public:
        Meta(Type type, std::string name, int version, RpcType rpc_type = GRPC);
        Meta(Type type, std::string name, int version, Flags *flags, RpcType rpc_type = GRPC);

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
        std::string tls_certificate_crt_path;

        /**
        * Necessary to provide when TLS enabled
        */
        std::string tls_certificate_key_path;

        /**
        * Neccessary to proivide when TLS Enabled.
        */
        std::string tls_certificate_authority_paths;

        /**
        * Enable stand-alone plugin
        */
        bool stand_alone;

        /**
        * Specify http port when stand-alone plugin is enabled
        */
        int stand_alone_port;
    };
```

An example using some arbitrary values:

```cpp
    Rando plg;
    Flags cli(argc, argv);
    Meta meta(Type::StreamCollector, "rando", 1, &cli, RpcType::GRPCStream);
    meta.concurrency_count = 1;

    start_stream_collector(&cli, &plg, meta);
```

## Testing

Official Snap plugins differentiate tests by scope into "small", "medium" and "large".
That's also the strategy recommended for plugins developed using this library.
For testing reference see the [Snap Testing Guidelines](https://github.com/intelsdi-x/snap/blob/master/CONTRIBUTING.md#testing-guidelines).
To test your plugin with Snap you will need to have [Snap](https://github.com/intelsdi-x/snap) installed,
check out these docs for [Snap setup details](https://github.com/intelsdi-x/snap/blob/master/docs/BUILD_AND_TEST.md#getting-started),
 keeping in mind it's targeting Go based plugins.
For C++ plugins test development consider using [Google Test framework](https://github.com/google/googletest),
which was selected to test this library itself.

## Ready to Share
You've made a plugin! Now it's time to share it. Create a release by following
these [steps](https://help.github.com/articles/creating-releases/). We recommend that your release version
 match your plugin version, as set in Plugin::Meta.

Don't forget to announce your plugin release on [slack](https://intelsdi-x.herokuapp.com/) and get your plugin added
 to the [Plugin Catalog](https://github.com/intelsdi-x/snap/blob/master/docs/PLUGIN_CATALOG.md)!
