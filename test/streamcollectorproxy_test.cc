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
#include <snap/config.h>
#include <snap/plugin.h>
#include <snap/proxy/stream_collector_proxy.h>
#include "gmock/gmock.h"

#include <sstream>
#include <string>
#include <vector>

#include "mocks.h"

using Plugin::Config;
using Plugin::ConfigPolicy;
using Plugin::Metric;
using Plugin::StringRule;
using Plugin::Proxy::StreamCollectorImpl;
using Plugin::StreamCollectorInterface;
using rpc::ConfigMap;
using ::testing::Return;
using ::testing::_;
using ::testing::Invoke;
using std::vector;

string extract_ns(const Metric &metric);
string extract_ns(const rpc::Metric &metric);

TEST(StreamCollectorProxySuccessTest, GetConfigPolicyWorks)
{
    MockStreamCollector mockee;
    ON_CALL(mockee, get_config_policy())
        .WillByDefault(Return(mockee.fake_policy));
    rpc::GetConfigPolicyReply resp;
    grpc::Status status;

    EXPECT_NO_THROW({
        StreamCollectorImpl streamCollector(&mockee);
        status = streamCollector.GetConfigPolicy(nullptr, nullptr, &resp);
    });
    EXPECT_EQ(grpc::StatusCode::OK, status.error_code());
    EXPECT_EQ(1, resp.string_policy_size());
}

TEST(StreamCollectorProxySuccessTest, GetMetricTypesWorks)
{
    MockStreamCollector mockee;
    rpc::MetricsReply resp;
    grpc::Status status;
    vector<Metric> fakeMetricTypes{mockee.fake_metric};

    ON_CALL(mockee, get_metric_types(_))
        .WillByDefault(Return(fakeMetricTypes));
    EXPECT_NO_THROW({
        StreamCollectorImpl streamCollector(&mockee);
        rpc::GetMetricTypesArg args;
        status = streamCollector.GetMetricTypes(nullptr, &args, &resp);
    });
    EXPECT_EQ(grpc::StatusCode::OK, status.error_code());
    EXPECT_EQ(1, resp.metrics_size());
    std::string ns_str = extract_ns(resp.metrics(0));
    EXPECT_EQ("/foo/bar", ns_str);
}

TEST(StreamCollectorProxySuccessTest, PingWorks)
{
    MockStreamCollector mockee;
    rpc::ErrReply resp;
    grpc::Status status;
    EXPECT_NO_THROW({
        StreamCollectorImpl streamCollector(&mockee);
        status = streamCollector.Ping(nullptr, nullptr, &resp);
    });
    EXPECT_EQ(grpc::StatusCode::OK, status.error_code());
}

TEST(StreamCollectorProxySuccessTest, KillWorks)
{
    MockStreamCollector mockee;
    rpc::ErrReply resp;
    grpc::Status status;
    EXPECT_NO_THROW({
        StreamCollectorImpl streamCollector(&mockee);
        status = streamCollector.Kill(nullptr, nullptr, &resp);
    });
    EXPECT_EQ(grpc::StatusCode::OK, status.error_code());
}

TEST(StreamCollectorProxyFailureTest, GetConfigPolicyReportsError)
{
    MockStreamCollector mockee;
    ON_CALL(mockee, get_config_policy())
        .WillByDefault(testing::Throw(Plugin::PluginException("nothing to look at")));
    rpc::GetConfigPolicyReply resp;
    grpc::Status status;
    EXPECT_NO_THROW({
        StreamCollectorImpl streamCollector(&mockee);
        status = streamCollector.GetConfigPolicy(nullptr, nullptr, &resp);
    });
    EXPECT_EQ(grpc::StatusCode::UNKNOWN, status.error_code());
    EXPECT_EQ("nothing to look at", status.error_message());
}
