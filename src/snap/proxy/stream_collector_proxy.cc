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
#include <vector>
#include <chrono>
#include <iostream>
#include <future>
#include <list>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "snap/proxy/stream_collector_proxy.h"
#include "snap/rpc/plugin.pb.h"
#include "snap/metric.h"

using google::protobuf::RepeatedPtrField;

using grpc::Server;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;
using grpc::StatusCode;

using rpc::Empty;
using rpc::ErrReply;
using rpc::GetConfigPolicyReply;
using rpc::GetMetricTypesArg;
using rpc::KillArg;
using rpc::MetricsArg;
using rpc::MetricsReply;
using rpc::CollectArg;
using rpc::CollectReply;

using Plugin::Metric;
using Plugin::PluginException;
using Plugin::Proxy::StreamCollectorImpl;

// Force instantiation of supported template types
//template class StreamCollectorImpl::StreamChannel<std::string>;
//template class StreamCollectorImpl::StreamChannel<std::vector<Metric>>;

#define DEFAULT_MAX_COLLECT_DURATION 10
#define DEFAULT_MAX_METRICS_BUFFER 0

StreamCollectorImpl::StreamCollectorImpl(Plugin::StreamCollectorInterface* plugin) :
                                        _stream_collector(plugin), 
                                        _max_collect_duration(DEFAULT_MAX_COLLECT_DURATION),
                                        _max_metrics_buffer(DEFAULT_MAX_METRICS_BUFFER) {
    _plugin_impl_ptr = new PluginImpl(plugin);
}

StreamCollectorImpl::~StreamCollectorImpl() {
    delete _plugin_impl_ptr;
}

Status StreamCollectorImpl::GetMetricTypes(ServerContext* context,
                                    const GetMetricTypesArg* req,
                                    MetricsReply* resp) {
    Plugin::Config cfg(req->config());

    try {
        std::vector<Metric> metrics = _stream_collector->get_metric_types(cfg);

        for (Metric met : metrics) {
            met.set_timestamp();
            met.set_last_advertised_time();
            *resp->add_metrics() = *met.get_rpc_metric_ptr();
        }
        return Status::OK;
    } catch (PluginException &e) {
        resp->set_error(e.what());
        return Status(StatusCode::UNKNOWN, e.what());
    }
}

Status SetConfig() {
    return Status::OK;
}

Status StreamCollectorImpl::Kill(ServerContext* context, const KillArg* req,
                        ErrReply* resp) {
    return _plugin_impl_ptr->Kill(context, req, resp);
}

Status StreamCollectorImpl::GetConfigPolicy(ServerContext* context, const Empty* req,
                                    GetConfigPolicyReply* resp) {
    try {
        return _plugin_impl_ptr->GetConfigPolicy(context, req, resp);
    } catch (PluginException &e) {
        resp->set_error(e.what());
        return Status(StatusCode::UNKNOWN, e.what());
    }
}

Status StreamCollectorImpl::Ping(ServerContext* context, const Empty* req,
                        ErrReply* resp) {
    return _plugin_impl_ptr->Ping(context, req, resp);
}

Status StreamCollectorImpl::StreamMetrics(ServerContext* context,
                ServerReaderWriter<CollectReply, CollectArg>* stream) {
    //std::cout << "debug: streaming started" << std::endl;
    try {
        std::string task_id = "not-set";

        std::vector<Metric> send_mets, recv_mets;
        std::string err_msg;

        auto sendch = std::async(std::launch::async, &StreamCollectorImpl::metricSend,
                                this, task_id, context, stream);
        auto recvch = std::async(std::launch::async, &StreamCollectorImpl::streamRecv,
                                this, task_id, context, stream);
        auto errch = std::async(std::launch::async, &StreamCollectorImpl::errorSend,
                                this, context, stream);

        auto do_puts = std::async(std::launch::async, &StreamCollectorImpl::PutSendMetsAndErrMsg,
                                this, context);

        _stream_collector->stream_metrics(send_mets, recv_mets, err_msg);

        return Status::OK;
    } catch(PluginException &e) {
        return Status(StatusCode::UNKNOWN, e.what());
    }
}

bool StreamCollectorImpl::PutSendMetsAndErrMsg(ServerContext* context) {
    while(!context->IsCancelled()) {
        if (_stream_collector->put_mets()) {
            _sendChan.put(_stream_collector->put_metrics_out());
            _stream_collector->set_put_mets(false);
        }
        if (_stream_collector->put_err()) {
            _errChan.put(_stream_collector->put_err_msg());
            _stream_collector->set_put_err(false);
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

bool StreamCollectorImpl::errorSend(ServerContext* context,
                                    ServerReaderWriter<CollectReply, CollectArg>* stream) {
    //std::cout << "Starting routine for sending errors" << std::endl;      
    try {
        CollectReply errReply;
        ErrReply er;
        std::string err;
        while (context->IsCancelled()) {
            if (_errChan.get(err)) {
                er.set_error(err);
                errReply.set_allocated_error(&er);
                stream->Write(errReply);
                err.clear();
            }
        }
        return true;
    } catch (PluginException &e) {
        std::cout << "Error" << std::endl;
        return false;
    }
}

bool StreamCollectorImpl::metricSend(const std::string &taskID,
                                    ServerContext* context,
                                    ServerReaderWriter<CollectReply, CollectArg>* stream) {
    MetricsReply* resp;
    std::vector<Metric> send_mets;
    //std::cout << "Starting routine for sending metrics" << std::endl;    
    try {
        std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
        while (!context->IsCancelled()) {
            if (_sendChan.get(send_mets)) {
                if (!send_mets.empty()) {
                    for (Metric met : send_mets) {
                        *resp->add_metrics() = *met.get_rpc_metric_ptr();

                        if (resp->metrics_size() == _max_metrics_buffer) {
                            sendReply(taskID, resp, stream);
                            resp->Clear();
                        }
                    }

                    if (_max_metrics_buffer == 0) {
                        sendReply(taskID, resp, stream);
                        resp->Clear();
                        start = std::chrono::system_clock::now();
                    }
                }
            }

            if ((std::chrono::system_clock::now() - start) >= _max_collect_duration) {
                sendReply(taskID, resp, stream);
                resp->Clear();
                start = std::chrono::system_clock::now();
            }
        }
        return true;
    } catch (PluginException &e) {
        std::cout << "Error" << std::endl;
        return false;
    }
}

bool StreamCollectorImpl::streamRecv(const std::string &taskID,
                                    ServerContext* context,
                                    ServerReaderWriter<CollectReply, CollectArg>* stream) {
    //std::cout << "Starting routine for receiving metrics" << std::endl; 
    try {
        std::vector<Metric> recv_mets;
        CollectArg collectMets;
        while (!context->IsCancelled()) {
            stream->Read(&collectMets);

            if (collectMets.maxcollectduration() > 0) {
                //std::cout << "Setting max-collect-duration" << std::endl;
                _max_collect_duration = std::chrono::seconds(collectMets.maxcollectduration());
            }
            if (collectMets.maxmetricsbuffer() > 0) {
                //std::cout << "Setting max-metrics-buffer" << std::endl;
                _max_metrics_buffer = collectMets.maxmetricsbuffer();
            }
            if (collectMets.has_metrics_arg()) {
                RepeatedPtrField<rpc::Metric> rpc_mets = collectMets.metrics_arg().metrics();

                for (int i = 0; i < rpc_mets.size(); i++) {
                    recv_mets.emplace_back(rpc_mets.Mutable(i));
                }

                _recvChan.put(recv_mets);
            }
        }
        _recvChan.close();
        return true;
    } catch (PluginException &e) {
        //std::cout << "Error" << std::endl;
        return false;
    }
}

bool StreamCollectorImpl::sendReply(const std::string &taskID,
                                    MetricsReply* resp,
                                    ServerReaderWriter<CollectReply, CollectArg>* stream) {
    //std::cout << "sendReply" << std::endl;
    try {
        if (resp->metrics_size() == 0) {
            //std::cout << "No metrics to send" << std::endl;
            return true;
        }
        CollectReply reply;
        reply.set_allocated_metrics_reply(resp);
        stream->Write(reply);
        return true;
    } catch (PluginException &e) {
        //std::cout << "Error" << std::endl;
        return false;
    }
}

template <class T>
void StreamCollectorImpl::StreamChannel<T>::close() {
    std::unique_lock<std::mutex> lock(_m);
    _closed = true;
    _cv.notify_all();
}

template <class T>
bool StreamCollectorImpl::StreamChannel<T>::is_closed() {
    std::unique_lock<std::mutex> lock(_m);
    return _closed;
}

template <class T>
void StreamCollectorImpl::StreamChannel<T>::put(const T &in) {
    std::unique_lock<std::mutex> lock(_m);
    
    if (_closed) throw std::logic_error("put to closed channel");
    
    _queue.push_back(in);
    _cv.notify_one();
}

template <class T>
bool StreamCollectorImpl::StreamChannel<T>::get(T &out, bool wait) {
    std::unique_lock<std::mutex> lock(_m);
    
    if (wait) _cv.wait(lock, [&]() { return _closed || !_queue.empty(); });
    if (_queue.empty()) return false;
    
    auto it = std::make_move_iterator(_queue.front().begin()),
        end = std::make_move_iterator(_queue.front().end());

    std::copy(it, end, std::back_inserter(out));

    _queue.pop_front();
    return true;
}
