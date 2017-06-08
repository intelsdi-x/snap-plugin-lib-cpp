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

using Plugin::Config;
using Plugin::ConfigPolicy;
using Plugin::Metric;
using Plugin::Meta;
using Plugin::Type;
using Plugin::Flags;

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
    std::vector<Metric> metrics = {
        {
            {
                {"intel", "", ""},
                {"cpp", "", ""},
                {"mock", "", ""},
                {"randomnumber", "", ""},
                {"float32", "", ""},
            },
            "",
            "float32 random number"
        },
        {
            {
                {"intel", "", ""},
                {"cpp", "", ""},
                {"mock", "", ""},
                {"randomnumber", "", ""},
                {"float64", "", ""},
            },
            "",
            "float64 random number"
        },
        {
            {
                {"intel", "", ""},
                {"cpp", "", ""},
                {"mock", "", ""},
                {"randomnumber", "", ""},
                {"int32", "", ""},
            },
            "",
            "int32 random number"
            },
        {
            {
                {"intel", "", ""},
                {"cpp", "", ""},
                {"mock", "", ""},
                {"randomnumber", "", ""},
                {"int64", "", ""},
            },
            "",
            "int64 random number"
        },
        {
            {
                {"intel", "", ""},
                {"cpp", "", ""},
                {"mock", "", ""},
                {"randomnumber", "", ""},
                {"uint32", "", ""},
            },
            "",
            "uint32 random number"
        },
        {
            {
                {"intel", "", ""},
                {"cpp", "", ""},
                {"mock", "", ""},
                {"randomnumber", "", ""},
                {"uint64", "", ""},
            },
            "",
            "uint64 random number"
        },
        {
            {
                {"intel", "", ""},
                {"cpp", "", ""},
                {"mock", "", ""},
                {"randomboolean", "", ""},
                {"boolean", "", ""},
            },
            "",
            "random boolean"
        },
        {
            {
                {"intel", "", ""},
                {"cpp", "", ""},
                {"mock", "", ""},
                {"randomstring", "", ""},
                {"string", "", ""},
            },
            "",
            "random string"
        }
    };
    return metrics;
}

void Rando::collect_metrics(std::vector<Metric> &metrics) {
    std::vector<Metric>::iterator mets_iter;
    unsigned int seed = time(NULL);
    int random_value = rand_r(&seed) % 1000;

    for (mets_iter = metrics.begin(); mets_iter != metrics.end(); mets_iter++) {
        std::string ns_mts_type = mets_iter->ns()[4].value;
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
        }
        mets_iter->set_timestamp();
    }
}

int main(int argc, char **argv) {
    Flags cli(argc, argv);
    Meta meta(Type::Collector, "rando", 1, &cli);

    if (cli.IsParsedFlag("version")) {
        cout << meta.name << " version "  << meta.version << endl;
        exit(0);
    }

    Rando plg;
    start_collector(&plg, meta);
}
