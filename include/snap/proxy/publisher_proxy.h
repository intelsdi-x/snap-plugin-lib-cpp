#include <grpc++/grpc++.h>

#include "plugin.grpc.pb.h"

#include "plugin.h"

using grpc::Server;
using grpc::ServerContext;
using grpc::Status;

using rpc::Publisher;
using rpc::MetricsArg;
using rpc::ErrReply;

namespace Plugin {
namespace Proxy {

class PublisherImpl final : Publisher::Service, Plugin {

  public:
    Status Publish(ServerContext* context, const MetricsArg* request,
                   ErrReply* response);
}

} // Proxy
} // Plugin
