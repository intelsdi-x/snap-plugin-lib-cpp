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

#include "snap/plugin.h"
#include "snap/config.h"
#include "snap/metric.h"
#include "gmock/gmock.h"

#include <string>
#include <vector>

using std::make_pair;
using std::pair;
using std::string;
using Plugin::Config;
using Plugin::Metric;
using Plugin::ConfigPolicy;
using Plugin::StringRule;
using Plugin::Namespace;
using Plugin::NamespaceElement;

class MockCollector : public Plugin::CollectorInterface {
public:
  ConfigPolicy fake_policy{Plugin::StringRule{
      "foo", {
          "bar",
          false
      }
  }};

  Metric fake_metric{ Namespace { {"foo","bar"}}, "" , ""};

  MOCK_METHOD0(get_config_policy, const ConfigPolicy());
  MOCK_METHOD1(get_metric_types, std::vector<Metric>(Config cfg));
  MOCK_METHOD1(collect_metrics, std::vector<Metric>(std::vector<Metric> &metrics));
};

class MockProcessor : public Plugin::ProcessorInterface {
public:
  ConfigPolicy fake_policy{Plugin::StringRule{
      "foo", {
          "bar",
          false
      }
  }};

  Metric fake_metric{ Namespace { {"foo","bar"}}, "" , ""};

  MOCK_METHOD0(get_config_policy, const ConfigPolicy());
  MOCK_METHOD2(process_metrics, void(std::vector<Plugin::Metric> &metrics, const Plugin::Config& config));
};

class MockPublisher : public Plugin::PublisherInterface {
public:
  ConfigPolicy fake_policy{Plugin::StringRule{
      "foo", {
          "bar",
          false
      }
  }};

  Metric fake_metric{ Namespace { {"foo","bar"}}, "" , ""};

  MOCK_METHOD0(get_config_policy, const ConfigPolicy());
  MOCK_METHOD2(publish_metrics, void(std::vector<Plugin::Metric> &metrics, const Plugin::Config& config));
};

class MockStreamCollector : public Plugin::StreamCollectorInterface
{
  public:
    ConfigPolicy fake_policy{Plugin::StringRule{
        "foo", {
            "bar",
            false
        }
    }};

    Metric fake_metric{Namespace{{"foo", "bar"}}, "", ""};

    MOCK_METHOD0(get_config_policy, const ConfigPolicy());
    MOCK_METHOD1(get_metric_types, std::vector<Metric>(Config cfg));

    MOCK_METHOD0(stream_metrics, void());

    MOCK_METHOD0(put_metrics_out, std::vector<Plugin::Metric>());
    MOCK_METHOD0(put_err_msg, std::string());
    MOCK_METHOD1(get_metrics_in, void(std::vector<Plugin::Metric> &metsIn));
    MOCK_METHOD0(put_mets, bool());
    MOCK_METHOD0(put_err, bool());
    MOCK_METHOD1(set_put_mets, void(const bool &putMets));
    MOCK_METHOD1(set_put_err, void(const bool &putErr));
    MOCK_METHOD1(set_context_cancelled, void(const bool &contextCancelled));
    MOCK_METHOD0(context_cancelled, bool());
};
