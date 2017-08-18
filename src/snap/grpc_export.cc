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
#include "snap/plugin.h"
#include "snap/grpc_export.h"
#include "snap/grpc_export_impl.h"

#include <chrono>
#include <fstream>
#include <future>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <sys/stat.h>

#include <grpc++/grpc++.h>
#include <json.hpp>
#include <boost/network/protocol/http/server.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>

#include "snap/rpc/plugin.pb.h"

#include "snap/proxy/collector_proxy.h"
#include "snap/proxy/processor_proxy.h"
#include "snap/proxy/publisher_proxy.h"

using std::cout;
using std::endl;
using std::future;
using std::runtime_error;
using std::string;
using std::shared_ptr;
using std::unique_ptr;

using grpc::Server;
using grpc::ServerBuilder;

using json = nlohmann::json;

using Plugin::PluginInterface;
using Plugin::Meta;
using Plugin::PluginException;

bool Plugin::GRPCExportImpl::file_exists(const std::string path) {
  bool retVal = false;
  if (FILE *file = fopen(path.c_str(), "r")) {
    fclose(file); retVal = true;
  }
  return retVal;
}

std::string Plugin::GRPCExportImpl::readFile(std::string path) {
  std::ifstream f(path.c_str());
  std::stringstream buffer;
  buffer << f.rdbuf();
  return buffer.str();
}

std::string Plugin::GRPCExportImpl::read_if_exists(std::string path) {
  std::string retVal("");
  if (file_exists(path)) {
    retVal = readFile(path);
  }
  return retVal;
}

std::string Plugin::GRPCExportImpl::load_key(std::string path) {
  std::string retVal(read_if_exists(path));
  if (retVal == "") {
    std::stringstream error{};
    error << "TLS Cert or Key at " << path << " does not exist or is empty.\n";
    throw PluginException(error.str());
  }
  return retVal;
}

std::string Plugin::GRPCExportImpl::load_directory(std::string path) {
  std::string retVal("");

  boost::filesystem::directory_iterator end_itr;

  if (boost::filesystem::is_directory(path)) {
    for(boost::filesystem::directory_iterator dir_itr(path); dir_itr != end_itr; dir_itr++) {
      if (boost::filesystem::is_regular_file(dir_itr->status())){
        retVal += load_key(dir_itr->path().string());
      }
      else {
        std::stringstream error{};
        error << "path found in folder, not a file: " << dir_itr->path().string() << std::endl;
        _logger->error(error.str());
      }
    }
  }
  return retVal;
}

std::string Plugin::GRPCExportImpl::load_tls_ca(std::string paths) {
  std::string retVal("");
  std::vector<std::string> results;
  boost::split(results, paths, [](char c){return c == ':';});

  struct stat s;
  for(auto it : results) {
    if( stat(it.c_str(), &s) == 0){
      if(s.st_mode & S_IFDIR) {
        retVal += load_directory(it);
      }
      else if (s.st_mode & S_IFREG ) {
        try {
          retVal += load_key(it);
        }
        catch (PluginException &e) {
          _logger->error(e.what());
        }
      }
      else {
        std::stringstream error{};
        error << "provided path is not a file or directory: " << it << std::endl;
        _logger->error(error.str());
      }
    }
    else {
      std::stringstream error{};
      error << "provided path does not resolve to a file or directory: " << it << std::endl;
      _logger->error(error.str());
    }
  }

  if (retVal == "") {
    std::stringstream error{};
    error << "found no useable certificates in given locations\n";
    throw PluginException(error.str());
  }
  return retVal;
}

namespace http = boost::network::http;

typedef http::server<Plugin::GRPCExportImpl::handler_type> http_server;

struct Plugin::GRPCExportImpl::handler_type {
    std::string preamble;
    void operator() (http_server::request const &request,
                     http_server::response &response) {
        response = http_server::response::stock_reply(
            http_server::response::ok, this->preamble);
    }

    void log(http_server::string_type const &info) {
        std::cerr << "Error: " << info << std::endl;
    }
};

Plugin::GRPCExporter::GRPCExporter() : impl(std::shared_ptr<GRPCExportImpl>(implement())) {}

future<void> Plugin::GRPCExporter::ExportPlugin(shared_ptr<PluginInterface> plugin, const Meta* meta) {
    return this->impl->DoExport(plugin, meta);
}

Plugin::GRPCExportImpl* Plugin::GRPCExporter::implement() {
    return new GRPCExportImpl();
}

future<void> Plugin::GRPCExportImpl::DoExport(shared_ptr<PluginInterface> plugin, const Meta *meta) {
    this->plugin = std::move(plugin);
    this->meta = meta;
    this->credentials = configureCredentials();
    doConfigure();
    doRegister();
    //_preamble = printPreamble();

    if (this->meta->stand_alone) {
        auto start_sa = std::async(std::launch::deferred, &Plugin::GRPCExportImpl::start_stand_alone, this,
                                    this->meta->stand_alone_port);
        start_sa.get();
    }
    else cout << printPreamble() << endl;
    
    auto self = this->shared_from_this();
    return std::async(std::launch::deferred, [=](){ self->doJoin(); });
}

std::shared_ptr<grpc::ServerCredentials> Plugin::GRPCExportImpl::configureCredentials() {
  auto retVal = grpc::InsecureServerCredentials();
  if (meta->tls_enabled) {
    setenv("GRPC_SSL_CIPHER_SUITES", "ECDHE-RSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384", 1);
    auto tls_key = load_key(this->meta->tls_certificate_key_path);
    auto tls_crt = load_key(this->meta->tls_certificate_crt_path);
    auto tls_ca = load_tls_ca(this->meta->tls_certificate_authority_paths);

    grpc::SslServerCredentialsOptions::PemKeyCertPair pkcp;
    pkcp = {tls_key, tls_crt};

    grpc::SslServerCredentialsOptions ssl_opts;
    ssl_opts.pem_key_cert_pairs.push_back(pkcp);
    ssl_opts.pem_root_certs = tls_ca;

    retVal =  grpc::SslServerCredentials(ssl_opts);
  }
  return retVal;
}


void Plugin::GRPCExportImpl::doConfigure() {
    std::stringstream ss;
    ss << this->meta->listen_addr << ":";
    this->meta->listen_port == "" ? ss << "0" : ss << this->meta->listen_port;

    switch (plugin->GetType()) {
        case Plugin::Collector:
            this->service.reset(new Proxy::CollectorImpl(plugin->IsCollector()));
            break;
        case Plugin::Processor:
            this->service.reset(new Proxy::ProcessorImpl(plugin->IsProcessor()));
            break;
        case Plugin::Publisher:
            this->service.reset(new Proxy::PublisherImpl(plugin->IsPublisher()));
            break;
    }
    
    builder.reset(new grpc::ServerBuilder());
    builder->AddListeningPort(ss.str(), this->credentials,
                            &this->port);
}

void Plugin::GRPCExportImpl::doRegister() {
    builder->RegisterService(service.get());
    this->server = std::move(builder->BuildAndStart());
}

json Plugin::GRPCExportImpl::printPreamble() {
    std::stringstream ss;
    ss << meta->listen_addr << ":" << port;

    json j = {
        {"Meta", {
                {"Type", meta->type},
                {"Name", meta->name},
                {"Version", meta->version},
                {"RPCType", meta->rpc_type},
                {"RPCVersion", RPC_VERSION},
                {"ConcurrencyCount", meta->concurrency_count},
                {"Exclusive", meta->exclusive},

                // The gRPC client in Snap does not use the `Unsecure` metadata key at
                // this time, as it is used for payload encryption.  With gRPC, encryption
                // is done via its transport, and this will be updated once support for
                // that feature lands in snapteld.
                {"Unsecure", meta->unsecure},
                {"CacheTTL", meta->cache_ttl.count()},
                {"RoutingStrategy", meta->strategy},
                {"PprofEnabled", meta->pprof_enabled},
                {"TLSEnabled", meta->tls_enabled},
                {"CertPath", meta->tls_certificate_crt_path},
                {"KeyPath", meta->tls_certificate_key_path},
                {"RootCertPaths", meta->tls_certificate_authority_paths},
                {"StandAloneEnabled", meta->stand_alone},
                {"StandAlonePort", meta->stand_alone_port},
                {"MaxCollectDuration", meta->max_collect_duration.count()},
                {"MaxMetricsBuffer", meta->max_metrics_buffer},
            }
        },
        {"ListenAddress", ss.str()},
        {"Type", meta->type},
        {"State", 0},
        {"ErrMessage", ""},
        {"Version", meta->version},
    };
    return j;
}

void Plugin::GRPCExportImpl::doJoin() {
    server->Wait();
}

int Plugin::GRPCExportImpl::start_stand_alone(const int &httpPort) {
    try {
        std::stringstream ss;
        ss << printPreamble();
  
        handler_type _handler;
        _handler.preamble = ss.str();
        http_server::options options(_handler);
        http_server _server(options.address("localhost").port(std::to_string(httpPort)));
        std::cout << "Preamble URL: " << "localhost:" << httpPort << std::endl;
        _server.run();
    }
    catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
