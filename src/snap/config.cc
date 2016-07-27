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

using Plugin::Config;
using Plugin::ConfigPolicy;
using Plugin::StringRule;

static const std::string build_key(const std::vector<std::string>&);

ConfigPolicy::ConfigPolicy() {}
ConfigPolicy::~ConfigPolicy() {}

ConfigPolicy::ConfigPolicy(const StringRule& rule) {
  add_rule({""}, rule);
}
ConfigPolicy::ConfigPolicy(const IntRule& rule) {
  add_rule({""}, rule);
}
ConfigPolicy::ConfigPolicy(const BoolRule& rule) {
  add_rule({""}, rule);
}

void ConfigPolicy::add_rule(const std::vector<std::string>& ns,
                            const StringRule& rule) {
  std::string key = build_key(ns);
  auto policy_ptr = mutable_string_policy();
  (*policy_ptr)[key] = rule;
}

void ConfigPolicy::add_rule(const std::vector<std::string>& ns,
                            const IntRule& rule) {
  std::string key = build_key(ns);
  auto policy_ptr = mutable_integer_policy();
  (*policy_ptr)[key]= rule;
}

void ConfigPolicy::add_rule(const std::vector<std::string>& ns,
                            const BoolRule& rule) {
  std::string key = build_key(ns);
  auto policy_ptr = mutable_bool_policy();
  (*policy_ptr)[key] = rule;
}

Config::Config(const rpc::ConfigMap& config) : rpc_map(config) {}

Config::~Config() {}

bool Config::get_bool(const std::string& key) const {
  auto bool_map = rpc_map.boolmap();
  return bool_map.at(key);
}

int Config::get_int(const std::string& key) const {
  auto int_map = rpc_map.intmap();
  return int_map.at(key);
}

std::string Config::get_string(const std::string& key) const {
  auto str_map = rpc_map.stringmap();
  return str_map.at(key);
}

static const std::string build_key(const std::vector<std::string>& ns) {
  std::stringstream ss;
  int i = 1;
  for (std::string node : ns) {
    if (i < ns.size()) ss << node << ".";
    if (i == ns.size()) ss << node;
    i++;
  }
  return ss.str();
}
