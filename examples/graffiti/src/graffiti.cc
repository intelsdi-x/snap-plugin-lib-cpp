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
#include "graffiti.h"

#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <snap/config.h>
#include <snap/plugin.h>
#include <snap/metric.h>

using Plugin::Config;
using Plugin::ConfigPolicy;
using Plugin::Metric;
using Plugin::Meta;
using Plugin::Type;

static inline std::vector<std::string> split_tags(std::string);

const ConfigPolicy Graffiti::get_config_policy() {
  ConfigPolicy policy(Plugin::StringRule{
    "tags",
    true
  });
  return policy;
}

void Graffiti::process_metrics(std::vector<Metric>* metrics,
                               const Config& config) {
  std::vector<Metric>::iterator mets_iter;
  std::string tags_str = config.get_string("tags");

  std::vector<std::string> tags = split_tags(tags_str);

  for (mets_iter = metrics->begin(); mets_iter != metrics->end(); mets_iter++) {
    for (std::string tag : tags) {
      mets_iter->add_tag(std::pair<std::string, std::string>(tag, "present"));
    }
  }
}

int main() {
  Meta meta(Type::Processor, "graffiti", 1);
  meta.unsecure = true;
  Graffiti plg = Graffiti();
  start_processor(&plg, meta);
}


static inline std::vector<std::string> split_tags(std::string tags_str) {
  std::stringstream tag_stream(tags_str);
  std::string elem;
  std::vector<std::string> tags;
  while (getline(tag_stream, elem, ',')) {
    if (!elem.empty()) tags.push_back(elem);
  }
  return tags;
}
