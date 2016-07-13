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

#ifndef PLUGIN_H
#define PLUGIN_H

#include <string>
#include <vector>

#include "snap/config.h"
#include "snap/metric.h"

namespace Plugin {

enum Type {
  collector,
  processor,
  publisher
};

class PluginInterface {

  public:
    virtual Config::Policy getConfigPolicy() = 0;
};

class CollectorInterface : public PluginInterface {

  public:
    virtual std::vector<class Metric::Metric> getMetricTypes(Config::Config
                                                             cfg) = 0;

    virtual std::vector<class Metric::Metric>
    collectMetrics(std::vector<class Metric::Metric> metrics) = 0;
};

class ProcessorInterface : public PluginInterface {

  public:
    virtual std::vector<class Metric::Metric> process(std::vector<class Metric::Metric> metrics) = 0;
};

class PublisherInterface : public PluginInterface {

  public:
    virtual void publish(std::vector<class Metric::Metric> metrics) = 0;
};

void start(PluginInterface* plg, Type plType, std::string name,
           int version);


}; // namespace Plugin

#endif
