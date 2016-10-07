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

#include <chrono>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "snap/rpc/plugin.pb.h"

#include "snap/config.h"

namespace Plugin {

/**
 * Metric is the representation of a Metric inside Snap.
 */
class Metric final {
 public:
  /**
   * A PODS describing an element in a metric namespace.
   */
  struct NamespaceElement {
    /**
     * value is the static value of this node in a namespace.
     * When a namespace element is _not_ dynamic, value is used. During
     * metric collection, value should contain the static name for this metric.
     */
    std::string value;

    /**
     * name is used to describe what this dynamic element is querying against.
     * E.g. in the namespace `/intel/kvm/[vm_id]/cpu_wait` the element at index
     * 2 has the name "vm_id".
     * @see value
     */
    std::string name;

    /**
     * description is the description of this namespace element.
     */
    std::string description;
  };

  enum DataType {
    String = rpc::Metric::DataCase::kStringData,
    Float32 = rpc::Metric::DataCase::kFloat32Data,
    Float64 = rpc::Metric::DataCase::kFloat64Data,
    Int32 = rpc::Metric::DataCase::kInt32Data,
    // TODO(danielscottt)
    // Int64 = rpc::Metric::DataCase::kInt64Data,
    // Bytes = rpc::Metric::DataCase::kBytesData,
    NotSet = rpc::Metric::DataCase::DATA_NOT_SET
  };

  Metric();
  /**
   * The typical metric constructor.
   * @param ns The metric's namespace.
   * @param unit The metric's unit.
   * @param description The metric's description.
   */
  Metric(std::vector<NamespaceElement> ns, std::string unit,
         std::string description);

  /**
   * This constructor is used in the plugin proxies.
   * It's used to wrap the rpc::Metric and rpc::ConfigMap types with the metric
   * type from this library.
   */
  explicit Metric(rpc::Metric* metric);

  Metric(const Metric& from);

  ~Metric();

  /**
   * ns returns the metric's namespace.
   * If there is a memoized copy, that is returned. Else the namespace is
   * copied into the cache then returned.
   */
  const std::vector<NamespaceElement>& ns() const;

  /**
   * dynamic_ns_elements returns the indices in the metric's namespace which
   * are dynamic.
   */
  std::vector<int> dynamic_ns_elements() const;

  /**
   * set_ns sets the namespace of the metric in its `rpc::Metric` ptr.
   * It also invalidates the memoization cache of the namespace if it is
   * present.
   * @see memo_ns
   */
  void set_ns(std::vector<NamespaceElement>);

  /**
   * tags returns the metric's tags.
   * If there is a memoized copy, that is returned. Else the tags are copied
   * into the cache then returned.
   */
  const std::map<std::string, std::string>& tags() const;

  /**
   * set_ns adds tags to the metric in its `rpc::Metric` ptr.
   * It also invalidates the memoization cache of the tags if it is
   * present.
   * @see memo_tags_ptr
   */
  void add_tag(std::pair<std::string, std::string>);

  /**
   * timestamp returns the metric's collection timestamp.
   */
  std::chrono::system_clock::time_point timestamp() const;
  /**
   * set_timestamp sets the timestamp as now.
   */
  void set_timestamp();

  /**
   * set_timestamp sets the timestamp as tp.
   * @param tp The timestamp to use.
   */
  void set_timestamp(std::chrono::system_clock::time_point tp);

  /**
   * set_last_advertised_time sets last_advertised_time as now.
   */
  void set_last_advertised_time();

  /**
   * set_last_advertised_time sets last_advertised_time as tp.
   * @param tp The timestamp to use.
   */
  void set_last_advertised_time(std::chrono::system_clock::time_point tp);

  DataType data_type();

  /**
   * set_data sets the metric instances data in the underlying rpc::Metric
   * pointer.
   */
  void set_data(int data);
  void set_data(float data);
  void set_data(double data);
  void set_data(const std::string& data);

  /**
   * Retrieve this metric's datapoint
   */
  int get_int_data() const;
  float get_float32_data() const;
  double get_float64_data() const;
  const std::string& get_string_data() const;
  Config get_config() const;
  const rpc::Metric* get_rpc_metric_ptr() const;

 private:
  rpc::Metric* rpc_metric_ptr;
  Config config;

  void inline set_ts(std::chrono::system_clock::time_point tp);
  void inline set_last_advert_tm(std::chrono::system_clock::time_point tp);

  // memoized members
  mutable std::vector<NamespaceElement> memo_ns;
  mutable std::map<std::string, std::string> memo_tags;

  bool delete_metric_ptr;
  DataType type;
};

}   // namespace Plugin
