#include <grpc++/grpc++.h>

#include "plugin.grpc.pb.h"

#include "plugin.h"

using grpc::Server;
using grpc::ServerContext;
using grpc::Status;

using rpc::Processor;
using rpc::MetricsArg;
using rpc::MetricsReply;

namespace Plugin {
namespace Proxy {

class ProcessorImpl final : Processor::Service, Plugin {

  public:
    Status Process(ServerContext* context, const MetricsArg* request,
                   MetricsReply* response);
}

} // Proxy
} // Plugin
