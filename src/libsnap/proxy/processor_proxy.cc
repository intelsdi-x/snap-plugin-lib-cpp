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

#include "plugin.grpc.pb.h"

#include "snap/proxy/processor_proxy.h"

using grpc::Server;
using grpc::ServerContext;
using grpc::Status;

using rpc::Processor;
using rpc::MetricsArg;
using rpc::MetricsReply;

using namespace Plugin::Proxy;

Status ProcessorImpl::Process(ServerContext* context, const MetricsArg* request,
                              MetricsReply* response) {
  return Status::OK;
}
