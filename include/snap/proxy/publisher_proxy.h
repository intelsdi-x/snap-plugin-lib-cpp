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

#include "snap/rpc/plugin.pb.h"
#include "snap/rpc/plugin.grpc.pb.h"

#include "snap/proxy/plugin_proxy.h"

namespace Plugin {
namespace Proxy {

class PublisherImpl final : rpc::Publisher::Service {
 public:
  grpc::Status Publish(grpc::ServerContext* context,
                       const rpc::MetricsArg* req,
                       rpc::ErrReply* resp);
};

}  // namespace Proxy
}  // namespace Plugin
