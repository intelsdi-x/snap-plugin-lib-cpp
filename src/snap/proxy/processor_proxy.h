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

#include <grpc++/grpc++.h>

#include "snap/rpc/plugin.grpc.pb.h"
#include "snap/rpc/plugin.pb.h"

#include "snap/proxy/plugin_proxy.h"

namespace Plugin {
namespace Proxy {

class ProcessorImpl final : public rpc::Processor::Service {
 public:
  explicit ProcessorImpl(Plugin::ProcessorInterface* plugin);

  ~ProcessorImpl();

  grpc::Status Process(grpc::ServerContext* context,
                       const rpc::PubProcArg* req,
                       rpc::MetricsReply* resp);

  grpc::Status Kill(grpc::ServerContext* context, const rpc::KillArg* request,
                    rpc::ErrReply* response);

  grpc::Status GetConfigPolicy(grpc::ServerContext* context,
                               const rpc::Empty* request,
                               rpc::GetConfigPolicyReply* resp);

  grpc::Status Ping(grpc::ServerContext* context, const rpc::Empty* request,
                    rpc::ErrReply* resp);

 private:
  Plugin::ProcessorInterface* processor;
  PluginImpl* plugin_impl_ptr;
};

}   // namespace Proxy
}   // namespace Plugin
