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
#include "log.h"

#include <chrono>
#include <ctime>
#include <fstream>
#include <map>
#include <string>
#include <vector>

#include <snap/config.h>
#include <snap/plugin.h>
#include <snap/metric.h>

using std::chrono::system_clock;
using std::chrono::duration_cast;
using std::chrono::milliseconds;

using Plugin::Config;
using Plugin::ConfigPolicy;
using Plugin::Metric;
using Plugin::Meta;
using Plugin::Type;

const ConfigPolicy Log::get_config_policy() {
  ConfigPolicy policy(Plugin::StringRule{
    "path",
    true
  });
  return policy;
}

/**
 * {ISO 8601 timestamp} {namespace} tags: [{tags}] data: {data}
 */
void Log::publish_metrics(std::vector<Metric>* metrics,
                          const Config& config) {
  std::string path = config.get_string("path");
  std::ofstream outfile;
  outfile.open(path, std::ios::app);

  std::vector<Metric>::iterator mets_iter;

  for (mets_iter = metrics->begin(); mets_iter != metrics->end(); mets_iter++) {
    // timestamp
    system_clock::time_point ts = mets_iter->timestamp();
    std::time_t c_ts = system_clock::to_time_t(ts);
    char str_time_b[50];
    if (std::strftime(str_time_b, sizeof(str_time_b),
                      "%F %T", std::gmtime(&c_ts))) {
      outfile << str_time_b << " ";
    }

    // namespace
    for (Metric::NamespaceElement nse : mets_iter->ns()) {
      outfile << "/" << nse.value;
    }

    // tags
    outfile << " tags: [";
    std::map<std::string, std::string>::iterator tags_iter;
    std::map<std::string, std::string> tags = mets_iter->tags();
    for (tags_iter = tags.begin(); tags_iter != tags.end();
         tags_iter++) {
      outfile << tags_iter->first << ", ";
    }

    // data
    outfile << "] " << "data: ";
    switch (mets_iter->data_type()) {
      case Metric::DataType::String:
        outfile << mets_iter->get_float64_data() << "\n";
        break;
      case Metric::DataType::Float32:
        outfile << mets_iter->get_float32_data() << "\n";
        break;
      case Metric::DataType::Float64:
        outfile << mets_iter->get_float64_data() << "\n";
        break;
      case Metric::DataType::Int32:
        outfile << mets_iter->get_int_data() << "\n";
        break;
      case Metric::DataType::NotSet:
        outfile << "not set\n";
        break;
    }
  }
}

int main() {
  Meta meta(Type::Publisher, "log", 1);
  meta.unsecure = true;
  Log plg = Log();
  start_publisher(&plg, meta);
}
