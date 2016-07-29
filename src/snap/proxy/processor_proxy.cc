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
#include "snap/proxy/processor_proxy.h"

#include <grpc++/grpc++.h>

#include "snap/rpc/plugin.pb.h"

using google::protobuf::RepeatedPtrField;

using grpc::Server;
using grpc::ServerContext;
using grpc::Status;

using rpc::Empty;
using rpc::ErrReply;
using rpc::GetConfigPolicyReply;
using rpc::KillArg;
using rpc::MetricsReply;
using rpc::Processor;
using rpc::PubProcArg;

using Plugin::Proxy::ProcessorImpl;

ProcessorImpl::ProcessorImpl(Plugin::ProcessorInterface* plugin) :
                             processor(plugin) {
  plugin_impl_ptr = new PluginImpl(plugin);
}

ProcessorImpl::~ProcessorImpl() {
  delete plugin_impl_ptr;
}

Status ProcessorImpl::Process(ServerContext* context, const PubProcArg* req,
                              MetricsReply* resp) {
  std::vector<Metric::Metric> metrics;
  RepeatedPtrField<rpc::Metric> rpc_mets = req->metrics();

  for (int i = 0; i < rpc_mets.size(); i++) {
    metrics.emplace_back(rpc_mets.Mutable(i));
  }

  Plugin::Config config(req->config());
  processor->process_metrics(&metrics, config);

  for (Metric::Metric met : metrics) {
    *resp->add_metrics() = *met.rpc_metric_ptr;
  }
  return Status::OK;
}

Status ProcessorImpl::Kill(ServerContext* context, const KillArg* req,
                           ErrReply* resp) {
  return plugin_impl_ptr->Kill(context, req, resp);
}

Status ProcessorImpl::GetConfigPolicy(ServerContext* context, const Empty* req,
                                      GetConfigPolicyReply* resp) {
  return plugin_impl_ptr->GetConfigPolicy(context, req, resp);
}

Status ProcessorImpl::Ping(ServerContext* context, const Empty* req,
                           ErrReply* resp) {
  return plugin_impl_ptr->Ping(context, req, resp);
}
