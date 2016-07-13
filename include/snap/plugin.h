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
