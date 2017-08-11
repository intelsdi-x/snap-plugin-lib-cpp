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
#include <snap/proxy/collector_proxy.h>
#include "gmock/gmock.h"

#include <sstream>
#include <string>
#include <vector>

#include "mocks.h"

using Plugin::Config;
using Plugin::ConfigPolicy;
using Plugin::Metric;
using Plugin::StringRule;
using Plugin::Proxy::CollectorImpl;
using rpc::ConfigMap;
using ::testing::Return;
using ::testing::_;
using ::testing::Invoke;
using std::vector;

string extract_ns(const Metric& metric);
string extract_ns(const rpc::Metric& metric);

TEST(CollectorProxySuccessTest, GetConfigPolicyWorks) {
    MockCollector mockee;
    ON_CALL(mockee, get_config_policy())
            .WillByDefault(Return(mockee.fake_policy));
    rpc::GetConfigPolicyReply resp;
    grpc::Status status;
    EXPECT_NO_THROW({
        CollectorImpl collector(&mockee);
        status = collector.GetConfigPolicy(nullptr, nullptr, &resp);
    });
    EXPECT_EQ(grpc::StatusCode::OK, status.error_code());
    EXPECT_EQ(1, resp.string_policy_size());
}

TEST(CollectorProxySuccessTest, GetMetricTypesWorks) {
    MockCollector mockee;
    rpc::MetricsReply resp;
    grpc::Status status;
    vector<Metric> fakeMetricTypes{mockee.fake_metric};

    ON_CALL(mockee, get_metric_types(_))
            .WillByDefault(Return(fakeMetricTypes));
    EXPECT_NO_THROW({
        CollectorImpl collector(&mockee);
        rpc::GetMetricTypesArg args;
        status = collector.GetMetricTypes(nullptr, &args, &resp);
    });
    EXPECT_EQ(grpc::StatusCode::OK, status.error_code());
    EXPECT_EQ(1, resp.metrics_size());
    std::string ns_str = extract_ns(resp.metrics(0));
    EXPECT_EQ("/foo/bar", ns_str);
}

TEST(CollectorProxySuccessTest, CollectMetricsWorks) {
    MockCollector mockee;
    rpc::MetricsReply resp;
    grpc::Status status;
    vector<Metric> fakeMetricTypes{mockee.fake_metric};
    vector<Metric> report;
    auto reporter = [&] (vector<Metric> &metrics) {
        for (int i =0; i < metrics.size(); i++) {
            const string data = "hop";
            metrics.at(i).set_data(data);
            report.emplace_back(metrics.at(i));
        }
        vector<Metric> result = metrics;
        return result;
    };

    ON_CALL(mockee, collect_metrics(_))
            .WillByDefault(Invoke(reporter));
    EXPECT_NO_THROW({
                        CollectorImpl collector(&mockee);
                        rpc::MetricsArg args;
                        *args.add_metrics() = *mockee.fake_metric.get_rpc_metric_ptr();
                        status = collector.CollectMetrics(nullptr, &args, &resp);
                    });
    EXPECT_EQ(grpc::StatusCode::OK, status.error_code());
    EXPECT_EQ(1, resp.metrics_size());
    EXPECT_EQ("hop", resp.metrics(0).string_data());
    std::string ns_str = extract_ns(*report.begin());
    EXPECT_EQ("/foo/bar", ns_str);
}

TEST(CollectorProxySuccessTest, PingWorks) {
    MockCollector mockee;
    rpc::ErrReply resp;
    grpc::Status status;
    EXPECT_NO_THROW({
                        CollectorImpl collector(&mockee);
                        status = collector.Ping(nullptr, nullptr, &resp);
                    });
    EXPECT_EQ(grpc::StatusCode::OK, status.error_code());
}

TEST(CollectorProxySuccessTest, KillWorks) {
    MockCollector mockee;
    rpc::ErrReply resp;
    grpc::Status status;
    EXPECT_NO_THROW({
                        CollectorImpl collector(&mockee);
                        status = collector.Kill(nullptr, nullptr, &resp);
                    });
    EXPECT_EQ(grpc::StatusCode::OK, status.error_code());
}

TEST(CollectorProxyFailureTest, GetConfigPolicyReportsError) {
    MockCollector mockee;
    ON_CALL(mockee, get_config_policy())
            .WillByDefault(testing::Throw(Plugin::PluginException("nothing to look at")));
    rpc::GetConfigPolicyReply resp;
    grpc::Status status;
    EXPECT_NO_THROW({
                        CollectorImpl collector(&mockee);
                        status = collector.GetConfigPolicy(nullptr, nullptr, &resp);
                    });
    EXPECT_EQ(grpc::StatusCode::UNKNOWN, status.error_code());
    EXPECT_EQ("nothing to look at", status.error_message());
}

TEST(CollectorProxyFailureTest, GetMetricTypesReportsError) {
    MockCollector mockee;
    rpc::MetricsReply resp;
    grpc::Status status;

    ON_CALL(mockee, get_metric_types(_))
            .WillByDefault(testing::Throw(Plugin::PluginException("nothing to look at")));
    EXPECT_NO_THROW({
                        CollectorImpl collector(&mockee);
                        rpc::GetMetricTypesArg args;
                        status = collector.GetMetricTypes(nullptr, &args, &resp);
                    });
    EXPECT_EQ(grpc::StatusCode::UNKNOWN, status.error_code());
    EXPECT_EQ("nothing to look at", status.error_message());
}

TEST(CollectorProxyFailureTest, CollectMetricsReportsError) {
    MockCollector mockee;
    rpc::MetricsReply resp;
    grpc::Status status;
    vector<Metric> fakeMetricTypes{mockee.fake_metric};

    ON_CALL(mockee, collect_metrics(_))
            .WillByDefault(testing::Throw(Plugin::PluginException("nothing to look at")));
    EXPECT_NO_THROW({
                        CollectorImpl collector(&mockee);
                        rpc::MetricsArg args;
                        *args.add_metrics() = *mockee.fake_metric.get_rpc_metric_ptr();
                        status = collector.CollectMetrics(nullptr, &args, &resp);
                    });
    EXPECT_EQ(grpc::StatusCode::UNKNOWN, status.error_code());
    EXPECT_EQ("nothing to look at", status.error_message());
}
