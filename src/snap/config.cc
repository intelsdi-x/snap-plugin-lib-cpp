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
#include "snap/config.h"

#include <sstream>
#include <string>
#include <vector>

using Plugin::ConfigPolicy;
using Plugin::StringRule;

ConfigPolicy::ConfigPolicy() {}
ConfigPolicy::~ConfigPolicy() {}

void ConfigPolicy::add_rule(const std::vector<std::string>& ns,
                            const StringRule& rule) {
  std::stringstream ss;
  int i = 1;
  for (std::string node : ns) {
    if (i < ns.size()) ss << node << ".";
    if (i == ns.size()) ss << node;
    i++;
  }
  auto policy_ptr = mutable_string_policy();
  (*policy_ptr)[ss.str()] = rule;
}
