lib_LTLIBRARIES = libsnap.la

nobase_include_HEADERS =         \
    snap/metric.h                \
    snap/config.h                \
    snap/plugin.h                \
    snap/proxy/plugin_proxy.h    \
    snap/proxy/collector_proxy.h \
    snap/proxy/processor_proxy.h \
    snap/proxy/publisher_proxy.h \
    snap/rpc/plugin.pb.h         \
    snap/rpc/plugin.grpc.pb.h

libsnap_la_SOURCES =              \
    snap/metric.cc                \
    snap/config.cc                \
    snap/plugin.cc                \
    snap/proxy/plugin_proxy.cc    \
    snap/proxy/collector_proxy.cc \
    snap/proxy/processor_proxy.cc \
    snap/proxy/publisher_proxy.cc \
    snap/rpc/plugin.pb.cc         \
    snap/rpc/plugin.grpc.pb.cc

libsnap_la_CPPFLAGS = \
    --std=c++11       \
    -I$(top_srcdir)/include
