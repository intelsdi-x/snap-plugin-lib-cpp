#include <snap/plugin>
#include <snap/config>

using Plugin::Collector;

using Config::Policy;
using Config::Config;

class Rando final : CollectorInterface {
  public:
    Policy getConfigPolicy();
    std::vector<Metric> getMetricTypes(Config cfg);
    std::vector<Metric> collectMetrics(std::vector<Metric> metrics);
}
