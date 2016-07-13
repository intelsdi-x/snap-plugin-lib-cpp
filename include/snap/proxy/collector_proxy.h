#include <grpc++/grpc++.h>

#include "plugin.grpc.pb.h"

#include "plugin.h"

using grpc::Server;
using grpc::ServerContext;
using grpc::Status;

using rpc::Collector;
using rpc::GetMetricTypesArg;
using rpc::MetricsArg;
using rpc::MetricsReply;

namespace Plugin {
namespace Proxy {

class CollectorImpl final : Collector::Service, PluginImpl {

  public:
    Status CollectMetrics(ServerContext* context, const MetricsArg* request,
                          MetricsReply* response);

    Status GetMetricTypes(ServerContext* context,
                          const GetMetricTypesArg* request,
                          MetricsReply* response);
}

} // Proxy
} // Plugin
