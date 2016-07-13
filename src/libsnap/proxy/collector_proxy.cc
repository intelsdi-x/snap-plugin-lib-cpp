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

using Plugin::Proxy;

Status CollectorImpl::CollectMetrics(ServerContext* context,
                                     const MetricsArg* request,
                                     MetricsReply* response) {
}

Status CollectorImpl::GetMetricTypes(ServerContext* context,
                                     const GetMetricTypesArg* request,
                                     MetricsReply* response) {
}
