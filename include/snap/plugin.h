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

#include <string>
#include <vector>

#include "plugin/config.h"
#include "plugin/metric.h"

using Metric::Metric;

namespace Plugin {

enum Type {
  collector,
  processor,
  publisher
};

public void start(PluginInterface* plg, Type plType, std::string name,
                  int version);

class PluginInterface {

  public:
    virtual Config::Policy getConfigPolicy() = 0;
}

class CollectorInterface : public PluginInterface {

  public:
    virtual std::vector<Metric> getMetricTypes(Config cfg) = 0;
    virtual std::vector<Metric> collectMetrics(std::vector<Metric> metrics) = 0;
}

class ProcessorInterface : public PluginInterface {

  public:
    virtual std::vector<Metric> process(std::vector<Metric> metrics) = 0;
}

class PublisherInterface : public PluginInterface {

  public:
    virtual publish(std::vector<Metric> metrics) = 0;
}

} // namespace Plugin
