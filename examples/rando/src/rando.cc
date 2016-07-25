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
#include <vector>

#include <snap/config.h>
#include <snap/plugin.h>
#include <snap/metric.h>

#include "rando.h"

using Plugin::Config;
using Plugin::ConfigPolicy;
using Plugin::Metric;
using Plugin::Meta;
using Plugin::Type;

ConfigPolicy Rando::get_config_policy() {
  ConfigPolicy policy;
  return policy;
}

std::vector<Metric> Rando::get_metric_types(Config cfg) {
  std::vector<Metric> metrics = {
    {
      {
        {"intel", "", ""},
        {"cpp", "", ""},
        {"mock", "", ""},
        {"random_number", "", ""},
        {"one", "", ""},
      },
      "",
      "the first random number"
    },
    {
      {
        {"intel", "", ""},
        {"cpp", "", ""},
        {"mock", "", ""},
        {"random_number", "", ""},
        {"two", "", ""},
      },
      "",
      "the second random number"
    }
  };
  return metrics;
}

void Rando::collect_metrics(std::vector<Metric>* metrics) {
  std::vector<Metric>::iterator mets_iter;
  unsigned int seed;
  for (mets_iter = metrics->begin(); mets_iter != metrics->end(); mets_iter++) {
    mets_iter->set_data(rand_r(&seed) % 1000);
    mets_iter->set_timestamp();
  }
}

int main() {
  Meta meta(Type::Collector, "rando", 1);
  meta.unsecure = true;
  Rando plg = Rando();
  start_collector(&plg, meta);
}
