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

#include "snap/plugin.h"

namespace Plugin {
namespace Proxy {

class PluginImpl final {
 public:
  explicit PluginImpl(Plugin::PluginInterface* plugin);

  grpc::Status Ping(grpc::ServerContext* context, const rpc::Empty* req,
                    rpc::ErrReply* resp);


  grpc::Status Kill(grpc::ServerContext* context, const rpc::KillArg* req,
                    rpc::ErrReply* response);

  grpc::Status GetConfigPolicy(grpc::ServerContext* context,
                               const rpc::Empty* req,
                               rpc::GetConfigPolicyReply* resp);

 private:
  Plugin::PluginInterface* plugin;
};

}  // namespace Proxy
}  // namespace Plugin
