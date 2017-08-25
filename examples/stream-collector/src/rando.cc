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
#include "rando.h"

#include <map>
#include <string>
#include <time.h>
#include <vector>
#include <iostream>

#include <snap/config.h>
#include <snap/plugin.h>
#include <snap/metric.h>
#include <snap/flags.h>
#include <snap/proxy/stream_collector_proxy.h>

using Plugin::Config;
using Plugin::ConfigPolicy;
using Plugin::Metric;
using Plugin::Meta;
using Plugin::Type;
using Plugin::Flags;
using Plugin::RpcType;
using Plugin::Namespace;
using Plugin::NamespaceElement;

using std::cout;
using std::endl;

enum supported_types
{
    float32,
    float64,
    int32,
    int64,
    uint32,
    uint64,
    boolean,
    string
};

std::map<std::string, int> metric_types{
    {"float32", supported_types::float32},
    {"float64", supported_types::float64},
    {"int32", supported_types::int32},
    {"int64", supported_types::int64},
    {"uint32", supported_types::uint32},
    {"uint64", supported_types::uint64},
    {"boolean", supported_types::boolean},
    {"string", supported_types::string}
};

const ConfigPolicy Rando::get_config_policy() {
    ConfigPolicy policy;
    policy.add_rule({"intel", "cpp"},
    Plugin::StringRule{
        "username",
        {"root", false}
    });
    policy.add_rule({"intel", "cpp", "mock", "randomnumber", "float32"},
    Plugin::StringRule{
        "password",
        {"h4ck3r", true}
    });
    return policy;
}

std::vector<Metric> Rando::get_metric_types(Config cfg) {
    std::vector<Metric> metrics = { Metric(Namespace({"intel","cpp","mock","rando"}).add_static_element("int32"), "example_unit","example_description" ),
                                    Metric(Namespace({"intel","cpp","mock","rando"}).add_static_element("int64"),"",""),
                                    Metric(Namespace({"intel","cpp","mock","rando"}).add_static_element("string"),"",""),
                                    Metric(Namespace({"intel","cpp","mock","rando"}).add_static_element("boolean"),"",""),
                                    };
    return metrics;
}

void Rando::stream_metrics() {
    auto stream = std::async(std::launch::async, &Rando::stream_it, this);
    drain_metrics();
}

void Rando::stream_it() {
    while (1) {
        if (!_metrics_out.empty() && !_put_mets && !_put_err && !_get_mets) {
            std::vector<Metric>::iterator mets_iter;
            unsigned int seed = time(NULL);
            int random_value = rand_r(&seed) % 1000;

            for (mets_iter = _metrics_out.begin(); mets_iter != _metrics_out.end(); mets_iter++) {
                std::string ns_mts_type = mets_iter->ns()[4].get_value();
                int mts_type = metric_types[ns_mts_type];

                switch(mts_type) {
                case supported_types::float32:
                    mets_iter->set_data((float)random_value);
                    break;
                case supported_types::float64:
                    mets_iter->set_data((double)random_value);
                    break;
                case supported_types::int32:
                    mets_iter->set_data((int32_t)random_value);
                    break;
                case supported_types::int64:
                    mets_iter->set_data((int64_t)random_value);
                    break;
                case supported_types::uint32:
                    mets_iter->set_data((uint32_t)random_value);
                    break;
                case supported_types::uint64:
                    mets_iter->set_data((uint64_t)random_value);
                    break;
                case supported_types::boolean:
                    mets_iter->set_data(random_value%2 ? true : false);
                    break;
                case supported_types::string:
                    mets_iter->set_data(std::to_string(random_value));
                    break;
                default:
                    _err_msg.assign("Invalid type");
                    _put_err = true;
                }
                mets_iter->set_timestamp();
            }
            _put_mets = true;
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        else {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
}

void Rando::drain_metrics() {
    while(1) {
        if (_get_mets && (!_put_mets || !_put_err)) {
            _err_msg.clear();
            _metrics_out.clear();
            std::copy(_metrics_in.begin(), _metrics_in.end(), std::back_inserter(_metrics_out));
            _get_mets = false;            
        }
    }
}

int main(int argc, char **argv) {
    Meta meta(Type::StreamCollector, "rando", 1, RpcType::GRPCStream);
    Rando plg;
    start_stream_collector(argc, argv, &plg, meta);
}
