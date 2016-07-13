#include <iostream>
#include <vector>

#include <snap/config>
#include <snap/plugin>
#include <snap/metric>

#include "rando.h"

using Config::Config;
using Config::Policy;
using Metric::Metric;
using Plugin::Type;

Policy Rando::getConfigPolicy() {
}

std::vector<Metric> Rando::getMetricTypes(Config cfg) {
}

std::vector<Metric> Rando::collectMetrics(std::vector<Metric> metrics) {
}

int main() {
  Rando plg = Rando();
  Plugin::start(plg&, Type::collector, "rando", 1);
  std::cout << "well that compiled" << std::endl;
}
