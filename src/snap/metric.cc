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

#include <ratio>

#include <google/protobuf/repeated_field.h>



using std::chrono::system_clock;
using std::chrono::duration_cast;
using std::chrono::nanoseconds;
using std::chrono::seconds;

using google::protobuf::Map;
using google::protobuf::RepeatedPtrField;

using Plugin::Metric;

Metric::Metric() : delete_metric_ptr(true),
                   rpc_metric_ptr(new rpc::Metric),
                   type(DataType::NotSet),
                   config(Config(rpc_metric_ptr->config())) {}

Metric::Metric(std::vector<Metric::NamespaceElement> ns, std::string unit,
               std::string description) :
                 delete_metric_ptr(true),
                 type(DataType::NotSet),
                 rpc_metric_ptr(new rpc::Metric),
                 config(Config(rpc_metric_ptr->config())) {
  rpc_metric_ptr->set_unit(unit);
  rpc_metric_ptr->set_description(description);
  set_ns(ns);
}

Metric::Metric(rpc::Metric* metric) :
                 rpc_metric_ptr(metric),
                 type(DataType::NotSet),
                 delete_metric_ptr(false),
                 config(Config(rpc_metric_ptr->config())) {}

Metric::Metric(const Metric& from) : delete_metric_ptr(true),
                                     config(from.config) {
  rpc_metric_ptr = new rpc::Metric;
  *rpc_metric_ptr = *from.rpc_metric_ptr;
}

Metric::~Metric() {
  if (delete_metric_ptr) {
    delete rpc_metric_ptr;
  }
}

void Metric::set_ns(std::vector<Metric::NamespaceElement> ns) {
  memo_ns.clear();
  rpc_metric_ptr->clear_namespace_();

  for (Metric::NamespaceElement ns_elem : ns) {
    rpc::NamespaceElement* rpc_elem = rpc_metric_ptr->add_namespace_();
    rpc_elem->set_name(ns_elem.name);
    rpc_elem->set_value(ns_elem.value);
    rpc_elem->set_description(ns_elem.description);
    memo_ns.push_back({
      rpc_elem->value(),
      rpc_elem->name(),
      rpc_elem->description(),
    });
  }
}

const std::vector<Metric::NamespaceElement>& Metric::ns() const {
  if (memo_ns.size() != 0) {
    return memo_ns;
  }

  RepeatedPtrField<rpc::NamespaceElement> rpc_ns = rpc_metric_ptr->namespace_();
  memo_ns.reserve(rpc_ns.size());

  for (rpc::NamespaceElement rpc_elem : rpc_ns) {
    memo_ns.push_back({
        rpc_elem.value(),
        rpc_elem.name(),
        rpc_elem.description()
    });
  }
  return memo_ns;
}

std::vector<int> Metric::dynamic_ns_elements() const {
  std::vector<int> idxs;
  const std::vector<Metric::NamespaceElement>& namesp = ns();
  int i = 0; 
  for (auto element : namesp) {
    if (!element.name.empty()) {
      idxs.push_back(i);    
    }
    i++;
  }
  return idxs;
}

void Metric::add_tag(std::pair<std::string, std::string> pair) {
  // invalidate memoized tags.
  std::map<std::string, std::string> memo_tags;
  Map<std::string, std::string>* rpc_tags = rpc_metric_ptr->mutable_tags();
  (*rpc_tags)[pair.first] = pair.second;
}

const std::map<std::string, std::string>& Metric::tags() const {
  if (memo_tags.size() != 0) {
    return memo_tags;
  }
  const Map<std::string, std::string>& rpc_tags = rpc_metric_ptr->tags();
  memo_tags = std::map<std::string, std::string>(rpc_tags.begin(),
                                                 rpc_tags.end());
  return memo_tags;
}

/**
 * rpc::Time is a structure containing seconds and nanoseconds. To retrieve an
 * accurate timestamp, these two counters must be summed.
 */
system_clock::time_point Metric::timestamp() const {
  // retrieve the tpc::Time
  rpc::Time rpc_tm = rpc_metric_ptr->timestamp();

  // Convert the seconds member to a timepoint.
  system_clock::time_point tp(seconds(rpc_tm.sec()));

  // retrieve and convert the nanoseconds from rpc_tm to a nanoseconds duration.
  nanoseconds rpc_nano_dur = nanoseconds(rpc_tm.nsec());

  // add the rpc nanoseconds to tp.
  return (tp + rpc_nano_dur);
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

Metric::DataType Metric::data_type() {
  return (Metric::DataType)rpc_metric_ptr->data_case();
}

void Metric::set_data(float data) {
  type = DataType::Float32;
  rpc_metric_ptr->set_float32_data(data);
}

void Metric::set_data(double data) {
  type = DataType::Float64;
  rpc_metric_ptr->set_float64_data(data);
}

void Metric::set_data(int32_t data) {
  type = DataType::Int32;
  rpc_metric_ptr->set_int32_data(data);
}

void Metric::set_data(int64_t data) {
  type = DataType::Int64;
  rpc_metric_ptr->set_int64_data(data);
}

void Metric::set_data(uint32_t data) {
  type = DataType::Uint32;
  rpc_metric_ptr->set_uint32_data(data);
}

void Metric::set_data(uint64_t data) {
  type = DataType::Uint64;
  rpc_metric_ptr->set_uint64_data(data);
}

void Metric::set_data(bool data) {
  type = DataType::Bool;
  rpc_metric_ptr->set_bool_data(data);
}

void Metric::set_data(const std::string& data) {
  type = DataType::String;
  rpc_metric_ptr->set_string_data(data);
}

int32_t Metric::get_int_data() const {
  return rpc_metric_ptr->int32_data();
}

int64_t Metric::get_int64_data() const {
  return rpc_metric_ptr->int64_data();
}

uint32_t Metric::get_uint32_data() const {
  return rpc_metric_ptr->uint32_data();
}

uint64_t Metric::get_uint64_data() const {
  return rpc_metric_ptr->uint64_data();
}

float Metric::get_float32_data() const {
  return rpc_metric_ptr->float32_data();
}

double Metric::get_float64_data() const {
  return rpc_metric_ptr->float64_data();
}

bool Metric::get_bool_data() const {
  return rpc_metric_ptr->bool_data();
}

const std::string& Metric::get_string_data() const {
  return rpc_metric_ptr->string_data();
}

Plugin::Config Metric::get_config() const {
  return config;
}

const rpc::Metric* Metric::get_rpc_metric_ptr() const {
  return rpc_metric_ptr;
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
