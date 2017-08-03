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
#include <grpc++/grpc++.h>
#include <vector>

#include "snap/rpc/plugin.pb.h"
#include "snap/proxy/collector_proxy.h"
#include "snap/metric.h"

using google::protobuf::RepeatedPtrField;

using grpc::Server;
using grpc::ServerContext;
using grpc::Status;
using grpc::StatusCode;

using rpc::Empty;
using rpc::ErrReply;
using rpc::GetConfigPolicyReply;
using rpc::GetMetricTypesArg;
using rpc::KillArg;
using rpc::MetricsArg;
using rpc::MetricsReply;

using Plugin::Metric;
using Plugin::Proxy::CollectorImpl;

CollectorImpl::CollectorImpl(Plugin::CollectorInterface* plugin) :
                                collector(plugin) {
    plugin_impl_ptr = new PluginImpl(plugin);
}

CollectorImpl::~CollectorImpl() {
    delete plugin_impl_ptr;
}

Status CollectorImpl::CollectMetrics(ServerContext* context,
                                    const MetricsArg* req,
                                    MetricsReply* resp) {
    std::vector<Metric> metrics;
    RepeatedPtrField<rpc::Metric> rpc_mets = req->metrics();

    for (int i = 0; i < rpc_mets.size(); i++) {
        metrics.emplace_back(rpc_mets.Mutable(i));
    }

    try {
        std::vector<Metric> result_metrics = collector->collect_metrics(metrics);

        for (Metric met : result_metrics) {
            *resp->add_metrics() = *met.get_rpc_metric_ptr();
        }
        return Status::OK;
    } catch (PluginException &e) {
        resp->set_error(e.what());
        return Status(StatusCode::UNKNOWN, e.what());
    }
}

Status CollectorImpl::GetMetricTypes(ServerContext* context,
                                    const GetMetricTypesArg* req,
                                    MetricsReply* resp) {
    Plugin::Config cfg(req->config());

    try {
        std::vector<Metric> metrics = collector->get_metric_types(cfg);

        for (Metric met : metrics) {
            met.set_timestamp();
            met.set_last_advertised_time();
            *resp->add_metrics() = *met.get_rpc_metric_ptr();
        }
        return Status::OK;
    } catch (PluginException &e) {
        resp->set_error(e.what());
        return Status(StatusCode::UNKNOWN, e.what());
    }
}

Status CollectorImpl::Kill(ServerContext* context, const KillArg* req,
                        ErrReply* resp) {
    return plugin_impl_ptr->Kill(context, req, resp);
}

Status CollectorImpl::GetConfigPolicy(ServerContext* context, const Empty* req,
                                    GetConfigPolicyReply* resp) {
    try {
        return plugin_impl_ptr->GetConfigPolicy(context, req, resp);
    } catch (PluginException &e) {
        resp->set_error(e.what());
        return Status(StatusCode::UNKNOWN, e.what());
    }
}

Status CollectorImpl::Ping(ServerContext* context, const Empty* req,
                        ErrReply* resp) {
    return plugin_impl_ptr->Ping(context, req, resp);
}
