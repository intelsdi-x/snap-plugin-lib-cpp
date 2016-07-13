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
