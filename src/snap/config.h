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
#pragma once

#include <string>
#include <vector>

#include <snap/rpc/plugin.pb.h>

namespace Plugin {

template<class P, class R, typename T> class Rule;
typedef Rule<rpc::StringPolicy, rpc::StringRule, std::string> StringRule;
typedef Rule<rpc::IntegerPolicy, rpc::IntegerRule, int> IntRule;
typedef Rule<rpc::BoolPolicy, rpc::BoolRule, bool> BoolRule;

template<class P, class R, typename T>
class Rule final : public P {
 private:
  class rule final : public R {
   public:
    rule() = delete;
    explicit rule(bool req) {
      this->set_required(req);
    }
    rule(const T& def, bool req) {
      this->set_has_default(true);
      this->set_default_(def);
      this->set_required(req);
    }
    rule(const T& def, bool req, T min, T max) {
      this->set_minimum(min);
      this->set_maximum(max);
      this->set_has_default(true);
      this->set_default_(def);
      this->set_required(req);
    }
    rule(bool req, T min, T max) {
      this->set_minimum(min);
      this->set_maximum(max);
      this->set_required(req);
    }

    ~rule() {}
  };

 public:
  Rule() = delete;

  Rule(const std::string& key, const rule& rl) {
    auto rule_ptr = this->mutable_rules();
    (*rule_ptr)[key] = rl;
  }

  ~Rule() {}
};

class ConfigPolicy final : public rpc::GetConfigPolicyReply {
 public:
  ConfigPolicy();
  ~ConfigPolicy();

  void add_rule(const std::vector<std::string>& ns, const StringRule& rule);
};

class Config {
};

};  // namespace Plugin
