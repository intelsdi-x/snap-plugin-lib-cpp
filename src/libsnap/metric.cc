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
#include "snap/metric.h"

#include <utility>
#include <chrono>
#include <ratio>
#include <vector>
#include <map>

#include <google/protobuf/repeated_field.h>

#include "snap/rpc/plugin.pb.h"


using std::chrono::system_clock;
using std::chrono::duration_cast;
using std::chrono::nanoseconds;

using google::protobuf::Map;
using google::protobuf::RepeatedPtrField;

using Plugin::Metric;

Metric::Metric() : delete_metric_ptr(true) {
  rpc_metric_ptr = new rpc::Metric;
}

Metric::Metric(std::vector<Metric::NamespaceElement> ns, std::string unit,
               std::string description) : delete_metric_ptr(true) {
  rpc_metric_ptr = new rpc::Metric;
  rpc_metric_ptr->set_unit(unit);
  rpc_metric_ptr->set_description(description);
  set_ns(ns);
}

Metric::Metric(rpc::Metric* metric) : rpc_metric_ptr(metric) {}

Metric::Metric(const Metric& from) : delete_metric_ptr(true) {
  rpc_metric_ptr = new rpc::Metric;
  *rpc_metric_ptr = *from.rpc_metric_ptr;
}

Metric::~Metric() {
  if (delete_metric_ptr) {
    delete rpc_metric_ptr;
  }
}

void Metric::set_ns(std::vector<Metric::NamespaceElement> ns) {
  std::vector<Metric::NamespaceElement> memo_ns;
  for (Metric::NamespaceElement ns_elem : ns) {
    rpc::NamespaceElement* rpc_elem = rpc_metric_ptr->add_namespace_();
    rpc_elem->set_name(ns_elem.name);
    rpc_elem->set_value(ns_elem.value);
    rpc_elem->set_description(ns_elem.description);
  }
}

const std::vector<Metric::NamespaceElement>& Metric::ns() {
  if (memo_ns.size() != 0) {
    return memo_ns;
  }

  RepeatedPtrField<rpc::NamespaceElement> rpc_ns = rpc_metric_ptr->namespace_();
  memo_ns.reserve(rpc_ns.size());

  for (rpc::NamespaceElement rpc_elem : rpc_ns) {
    memo_ns.push_back({
        rpc_elem.name(),
        rpc_elem.value(),
        rpc_elem.description()
    });
  }
  return memo_ns;
}

void Metric::add_tag(std::pair<std::string, std::string> pair) {
  // invalidate memoized tags.
  std::map<std::string, std::string> memo_tags;
  Map<std::string, std::string>* rpc_tags = rpc_metric_ptr->mutable_tags();
  (*rpc_tags)[pair.first] = pair.second;
}

const std::map<std::string, std::string>& Metric::tags() {
  if (memo_tags.size() != 0) {
    return memo_tags;
  }
  const Map<std::string, std::string>& rpc_tags = rpc_metric_ptr->tags();
  memo_tags = std::map<std::string, std::string>(rpc_tags.begin(),
                                                 rpc_tags.end());
  return memo_tags;
}

void Metric::set_timestamp() {
  auto now = system_clock::now();
  set_ts(now);
}

void Metric::set_timestamp(system_clock::time_point tp) {
  set_ts(tp);
}

void Metric::set_last_advertised_time() {
  auto now = system_clock::now();
  set_last_advert_tm(now);
}

void Metric::set_last_advertised_time(system_clock::time_point tp) {
  set_last_advert_tm(tp);
}

// TODO(danielscottt): figure out the whole ::google::protobuf::int{32,64}
// thing
void Metric::set_data(float data) {
  rpc_metric_ptr->set_float32_data(data);
}

void Metric::set_data(double data) {
  rpc_metric_ptr->set_float64_data(data);
}

void Metric::set_data(int data) {
  rpc_metric_ptr->set_int32_data(data);
}

void Metric::set_data(const std::string& data) {
  rpc_metric_ptr->set_string_data(data);
}

void Metric::set_ts(system_clock::time_point tp) {
  rpc::Time* tm = rpc_metric_ptr->mutable_timestamp();
  uint64_t nanos = uint64_t(duration_cast<nanoseconds>(
                         tp.time_since_epoch()).count());
  tm->set_sec(nanos / std::nano::den);
  tm->set_nsec(nanos % std::nano::den);
}

void Metric::Metric::set_last_advert_tm(system_clock::time_point tp) {
  rpc::Time* tm = rpc_metric_ptr->mutable_lastadvertisedtime();
  uint64_t nanos = uint64_t(duration_cast<nanoseconds>(
                         tp.time_since_epoch()).count());
  tm->set_sec(nanos / std::nano::den);
  tm->set_nsec(nanos % std::nano::den);
}
