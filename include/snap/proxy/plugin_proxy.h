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
