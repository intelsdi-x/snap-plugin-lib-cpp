# http://www.apache.org/licenses/LICENSE-2.0.txt
#
#
# Copyright 2016 Intel Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
lib_LTLIBRARIES = libsnap.la

nobase_include_HEADERS =               \
    snap/metric.h                      \
    snap/config.h                      \
    snap/grpc_export.h                 \
    snap/grpc_export_impl.h            \
    snap/plugin.h                      \
    snap/lib_setup_impl.h              \
    snap/flags.h                       \
    snap/proxy/plugin_proxy.h          \
    snap/proxy/collector_proxy.h       \
    snap/proxy/processor_proxy.h       \
    snap/proxy/publisher_proxy.h       \
    snap/proxy/stream_collector_proxy.h \
    snap/rpc/plugin.pb.h               \
    snap/rpc/plugin.grpc.pb.h

libsnap_la_SOURCES =                    \
    snap/metric.cc                      \
    snap/config.cc                      \
    snap/grpc_export.cc                 \
    snap/plugin.cc                      \
    snap/flags.cc                       \
    snap/proxy/plugin_proxy.cc          \
    snap/proxy/collector_proxy.cc       \
    snap/proxy/processor_proxy.cc       \
    snap/proxy/publisher_proxy.cc       \
    snap/proxy/stream_collector_proxy.cc \
    snap/rpc/plugin.pb.cc               \
    snap/rpc/plugin.grpc.pb.cc

libsnap_la_CPPFLAGS =       \
    --std=c++1y             \
    -I$(top_srcdir)/include 

