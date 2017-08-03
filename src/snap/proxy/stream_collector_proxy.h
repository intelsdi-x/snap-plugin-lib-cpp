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
#include <chrono>
#include <vector>
#include <list>
#include <thread>
#include <mutex>
#include <condition_variable>

#include "snap/rpc/plugin.grpc.pb.h"
#include "snap/rpc/plugin.pb.h"

#include "snap/proxy/plugin_proxy.h"

namespace Plugin {
    namespace Proxy {
        class StreamCollectorImpl final : public rpc::StreamCollector::Service {
        public:
            explicit StreamCollectorImpl(Plugin::StreamCollectorInterface* plugin);

            ~StreamCollectorImpl();

            grpc::Status GetMetricTypes(grpc::ServerContext* context,
                                        const rpc::GetMetricTypesArg* request,
                                        rpc::MetricsReply* resp);

            grpc::Status SetConfig();

            grpc::Status Kill(grpc::ServerContext* context, const rpc::KillArg* request,
                            rpc::ErrReply* response);

            grpc::Status GetConfigPolicy(grpc::ServerContext* context,
                                        const rpc::Empty* request,
                                        rpc::GetConfigPolicyReply* resp);

            grpc::Status Ping(grpc::ServerContext* context, const rpc::Empty* request,
                            rpc::ErrReply* resp);

            grpc::Status StreamMetrics(grpc::ServerContext* context,
                            grpc::ServerReaderWriter<rpc::CollectReply, rpc::CollectArg>* stream);

            void SetMaxCollectDuration(std::chrono::seconds maxCollectDuration) {
                _max_collect_duration = maxCollectDuration;
            }
            std::chrono::seconds GetMaxCollectDuration () { 
                return _max_collect_duration; 
            }
            void SetMaxMetricsBuffer(int64_t maxMetricsBuffer) {
                _max_metrics_buffer = maxMetricsBuffer;
            }
            int64_t GetMaxMetricsBuffer() { 
                return _max_metrics_buffer; 
            }

            bool errorSend(grpc::ServerContext* context,
                            grpc::ServerReaderWriter<rpc::CollectReply, rpc::CollectArg>* stream);
            bool metricSend(const std::string &taskID,
                            grpc::ServerContext* context,
                            grpc::ServerReaderWriter<rpc::CollectReply, rpc::CollectArg>* stream);
            bool streamRecv(const std::string &taskID,
                            grpc::ServerContext* context,
                            grpc::ServerReaderWriter<rpc::CollectReply, rpc::CollectArg>* stream);

            bool sendReply(const std::string &taskID,
                            rpc::MetricsReply *resp,
                            grpc::ServerReaderWriter<rpc::CollectReply, rpc::CollectArg>* stream);

            bool PutSendMetsAndErrMsg(grpc::ServerContext* context);
            
            void ErrChanClose() {
                _errChan.close();
            }
            bool ErrChanIsClosed() {
                return _errChan.is_closed();
            }
            void ErrChanPut(const std::string &errMsg) {
                _errChan.put(errMsg);
            }
            bool ErrChanGet(std::string &errMsg) {
                return _errChan.get(errMsg);
            }

            void SendChanClose() {
                _sendChan.close();
            }
            bool SendChanIsClosed() {
                return _sendChan.is_closed();
            }
            void SendChanPut(const std::vector<Metric> &metrics) {
                _sendChan.put(metrics);
            }
            bool SendChanGet(std::vector<Metric> &metrics) {
                return _sendChan.get(metrics);
            }

            void RecvChanClose() {
                _recvChan.close();
            }
            bool RecvChanIsClosed() {
                return _recvChan.is_closed();
            }
            void RecvChanPut(const std::vector<Metric> &metrics) {
                _recvChan.put(metrics);
            }
            bool RecvChanGet(std::vector<Metric> &metrics) {
                return _recvChan.get(metrics);
            }

            template <class T>
            class StreamChannel {
            private:
                std::list<T> _queue;
                std::mutex _m;
                std::condition_variable _cv;
                bool _closed;

            public:
                StreamChannel() : _closed(false) {}

                void close();
                bool is_closed();
                void put(const T &in);
                bool get(T &out, bool wait = false);
            };

        private:
            Plugin::StreamCollectorInterface* _stream_collector;
            PluginImpl* _plugin_impl_ptr;
            grpc::ServerContext* _ctx;
            int64_t _max_metrics_buffer;
            std::chrono::seconds _max_collect_duration;
            
            StreamChannel<std::vector<Plugin::Metric>> _sendChan;
            StreamChannel<std::vector<Plugin::Metric>> _recvChan;
            StreamChannel<std::string> _errChan;          
        };
    }  // namespace Proxy
}  // namespace Plugin
