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

using Plugin::Proxy;

Status PluginImpl::Ping(ServerContext* context, const Empty* request,
                        ErrReply* response) {
}

Status PluginImpl::Kill(ServerContext* context, const KillArg* request,
                        ErrReply* response) {
}

Status PluginImpl::GetConfigPolicy(ServerContext* context, const Empty* request,
                                   GetConfigPolicyReply* response) {
}

void PluginImpl::start() {
}
