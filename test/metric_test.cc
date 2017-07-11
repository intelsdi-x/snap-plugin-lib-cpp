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
#include "snap/metric.h"
#include "gtest/gtest.h"

#include <chrono>
#include <ctime>
#include <sstream>
#include <string>
#include <vector>

using std::chrono::system_clock;
using std::make_pair;
using std::pair;
using std::string;
using Plugin::Metric;
using Plugin::ConfigPolicy;
using Plugin::StringRule;


string extract_ns(const Metric& metric) {
    string ns_str;
    for (auto const& elem : metric.ns()) {
        ns_str += "/" + elem.value;
    }
    return ns_str;
}

string extract_ns(const rpc::Metric& metric) {
    string ns_str;
    for (int i = 0; i < metric.namespace__size(); i++) {
        ns_str += "/" + metric.namespace_(i).value();
    }
    return ns_str;
}

TEST(MetricTest, SetNsUnitDescriptionWorks) {
    Metric fake_metric{
            {
                    {"foo", "", ""},
                    {"bar", "", ""},
            },
            "atoms",
            "critical metric"
    };
    fake_metric.add_tag(make_pair("host", "baz"));
    fake_metric.add_tag(make_pair("node", "bonk"));

    EXPECT_EQ("/foo/bar", extract_ns(fake_metric));
    EXPECT_EQ("atoms", fake_metric.get_rpc_metric_ptr()->unit());
    EXPECT_EQ("critical metric", fake_metric.get_rpc_metric_ptr()->description());
    EXPECT_EQ("baz", fake_metric.get_rpc_metric_ptr()->tags().at("host"));
    EXPECT_EQ("bonk", fake_metric.get_rpc_metric_ptr()->tags().at("node"));
}

TEST(MetricTest, SetNsWorks) {
    Metric fake_metric;
    fake_metric.set_ns({
                               {"foo", "", ""},
                               {"cluster", "", ""},
                               {"node_count", "", ""}
                       });
    EXPECT_EQ("/foo/cluster/node_count", extract_ns(fake_metric));
}

TEST(MetricTest, SetFromRpcMetricWorks) {
    Metric base_metric{
            {
                    {"foo", "", ""},
                    {"bar", "", ""},
            },
            "atoms",
            "critical metric"
    };
    base_metric.add_tag(make_pair("host", "baz"));
    base_metric.add_tag(make_pair("node", "bonk"));

    rpc::Metric source_metric(*base_metric.get_rpc_metric_ptr());
    Metric fake_metric(&source_metric);
    EXPECT_EQ("/foo/bar", extract_ns(fake_metric));
    EXPECT_EQ("atoms", fake_metric.get_rpc_metric_ptr()->unit());
    EXPECT_EQ("critical metric", fake_metric.get_rpc_metric_ptr()->description());
    EXPECT_EQ("baz", fake_metric.get_rpc_metric_ptr()->tags().at("host"));
    EXPECT_EQ("bonk", fake_metric.get_rpc_metric_ptr()->tags().at("node"));
}

TEST(MetricTest, SetTagsWorks) {
    Metric fake_metric;
    fake_metric.add_tag(make_pair("host", "zero"));
    fake_metric.add_tag(make_pair("period", "1hr"));
    EXPECT_EQ("zero", fake_metric.tags().at("host"));
    EXPECT_EQ("1hr", fake_metric.tags().at("period"));
}

TEST(MetricTest, SetTimestampWorks) {
    Metric fake_metric;
    std::tm source_time{56,10,8,2,5,92,6,122,1}; // 02 May 1992, 08:10:56
    std::time_t raw_time = mktime(&source_time);
    fake_metric.set_timestamp(system_clock::from_time_t(raw_time));
    std::time_t metric_rawtime = system_clock::to_time_t(fake_metric.timestamp());
    tm metric_time = *gmtime(&metric_rawtime);
    EXPECT_EQ(92, metric_time.tm_year);
    EXPECT_EQ(5, metric_time.tm_mon);
    EXPECT_EQ(2, metric_time.tm_mday);
    EXPECT_EQ(10, metric_time.tm_min);
    EXPECT_EQ(56, metric_time.tm_sec);
}

TEST(MetricTest, SetLastAdvertisedTimeWorks) {
    Metric fake_metric;
    std::tm source_time{13,55,15,29,9,8,1,272,1}; // Mon, 29 Sep 2008 15:55:13 -0400
    std::time_t raw_time = mktime(&source_time);
    // set the expected value
    fake_metric.set_last_advertised_time(system_clock::from_time_t(raw_time));
    // obtain the actual value
    rpc::Time metric_rpctime = fake_metric.get_rpc_metric_ptr()->lastadvertisedtime();
    system_clock::time_point metric_timestamp(std::chrono::seconds(metric_rpctime.sec()));
    metric_timestamp += std::chrono::nanoseconds(metric_rpctime.nsec());
    std::time_t metric_rawtime = system_clock::to_time_t(metric_timestamp);
    tm metric_time = *gmtime(&metric_rawtime);
    EXPECT_EQ(8, metric_time.tm_year);
    EXPECT_EQ(9, metric_time.tm_mon);
    EXPECT_EQ(29, metric_time.tm_mday);
    EXPECT_EQ(55, metric_time.tm_min);
    EXPECT_EQ(13, metric_time.tm_sec);
}

TEST(MetricTest, SetDataWorks) {
    Metric fake_metric;

    const string string_var = "hop";
    fake_metric.set_data(string_var);
    EXPECT_EQ(string_var, fake_metric.get_string_data());

    float float_var = 3.14159;
    fake_metric.set_data(float_var);
    EXPECT_EQ(float_var, fake_metric.get_float32_data());

    double double_var = 1.73205;
    fake_metric.set_data(double_var);
    EXPECT_EQ(double_var, fake_metric.get_float64_data());

    int int_var = 40990;
    fake_metric.set_data(int_var);
    EXPECT_EQ(int_var, fake_metric.get_int_data());

    long int long_int_var = 40991;
    fake_metric.set_data(long_int_var);
    EXPECT_EQ(long_int_var, fake_metric.get_int64_data());

    unsigned int uint_var = 40992;
    fake_metric.set_data(uint_var);
    EXPECT_EQ(uint_var, fake_metric.get_uint32_data());

    unsigned long int ulong_int_var = 40993;
    fake_metric.set_data(ulong_int_var);
    EXPECT_EQ(ulong_int_var, fake_metric.get_uint64_data());
}
