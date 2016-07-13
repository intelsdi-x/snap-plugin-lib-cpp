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

#include "plugin.h"

using grpc::Server;
using grpc::ServerContext;
using grpc::Status;

using rpc::Empty;
using rpc::ErrReply;
using rpc::KillArg;
using rpc::GetConfigPolicyReply;

namespace Plugin {
namespace Proxy {

class PluginImpl {

  public:
    Plugin();
    ~Plugin();

    Status Ping(ServerContext* context, const Empty* request,
                ErrReply* response);

    Status Kill(ServerContext* context, const KillArg* request,
                ErrReply* response);

    Status GetConfigPolicy(ServerContext* context, const Empty* request,
                           GetConfigPolicyReply* response);

    void start()

  protected:
    ::Plugin::PluginInterface plugin;

}

} // Proxy
} // Plugin
