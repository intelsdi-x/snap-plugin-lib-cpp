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
#include <cstdint>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "snap/rpc/plugin.pb.h"

#include "snap/config.h"

namespace Plugin {

    class NamespaceElement{
        public:

        /**
        * Constructor which makes "NamespaceElement" from three strings:
        * value,
        * name (optional- default parameter is empty),
        * description (optional- default parameter is empty).
        */
        NamespaceElement(std::string val, std::string nam ="", std::string desc="");

        /**
        * Default empty constructor.
        */
        NamespaceElement();

        /**
        * Default empty destructor.
        */
        ~NamespaceElement();

        /**
        * Setters for value, name and description
        */
        void set_value(std::string v);
        void set_name(std::string n);
        void set_description(std::string d);

        /**
        * Getters for value, name and description
        */
        const std::string get_value() const;
        const std::string get_name() const;
        const std::string get_description() const;

        /**
        * is_dynamic returns true if the namespace element contains data.  A namespace
          element that has a nonempty Name field is considered dynamic.
        */
        const bool is_dynamic() const;

        private:
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

   class Namespace {
        public:

        /**
        * Constructor which makes namespace elements from vector of strings.
        */
        Namespace(std::vector<std::string> ns);

        /**
        * Default empty constructor.
        */
        Namespace();

        /**
        * Default empty destructor.
        */
        ~Namespace();

        /**
        * Overloaded range operators. They return "NamespaceElement" object
        * from given index.
        */
        const NamespaceElement operator[] (int index) const;

        const std::string get_string() const;

        NamespaceElement& operator[] (int index);

        /**
        *  add_static_element adds a static element to the Namespace.  A static
        *  namespaceElement is defined by having an empty Name field.
        */
        Namespace& add_static_element(std::string value);

        /**
        * add_dynamic_element adds a dynamic element to the given Namespace.  A dynamic
        * namespaceElement is defined by having a nonempty Name field.
        */
        Namespace& add_dynamic_element(std::string name ,std::string description ="");

        /**
        * Getter for vector of "NamespaceElements"
        */
        std::vector<NamespaceElement> get_namespace_elements() const;

        /**
        * is_dynamic returns bool (true when this namespace is dynamic)
        * and vector of indexes which elements are dynamic inside this "Namespace".
        * A dynamic component of the namespace are those elements that
        * contain variable data.
        */
        const bool is_dynamic() const;

        /**
        * get_dynamic_indexes returns vector of indexes which elements are dynamic
        * inside this "Namespace". A dynamic component of the namespace are those
        * elements that
        * A dynamic component of the namespace are those elements that contain variable data.
        */
        const std::vector<int> get_dynamic_indexes();
        
        /**
        * Clears vector of NamespaceElements.
        */
        void clear();

        /**
        * Returns size of vector of NamespaceElements.
        */
        unsigned int size() const;

        /**
        * Adds NamespaceElement to the end of exesting vector.
        */
        void push_back(NamespaceElement& element);

        /**
        * Same as above but this method takes rvalue of NamespaceElement.
        */
        void push_back(NamespaceElement&& element);

        /**
        * Reserves some size to the existing vector.
        */
        void reserve(unsigned int size);

        private:
        /**
        * This field holds all object's "namespace_elements" inside std::vector.
        */
        std::vector<NamespaceElement> namespace_elements;

    };


    /**
    * Metric is the representation of a Metric inside Snap.
    */
    class Metric final {
    public:

        enum DataType {
            String = rpc::Metric::DataCase::kStringData,
            Float32 = rpc::Metric::DataCase::kFloat32Data,
            Float64 = rpc::Metric::DataCase::kFloat64Data,
            Int32 = rpc::Metric::DataCase::kInt32Data,
            Int64 = rpc::Metric::DataCase::kInt64Data,
            Uint32 = rpc::Metric::DataCase::kUint32Data,
            Uint64 = rpc::Metric::DataCase::kUint64Data,
            Bool = rpc::Metric::DataCase::kBoolData,
            NotSet = rpc::Metric::DataCase::DATA_NOT_SET,
        };

        friend std::ostream& operator<<(std::ostream& lhs, DataType e) {
            switch(e) {
                case String : lhs << "String"; break;
                case Float32 : lhs << "Float32"; break;
                case Float64 : lhs << "Float64"; break;
                case Int32 : lhs << "Int32"; break;
                case Int64 : lhs << "Int64"; break;
                case Uint32 : lhs << "Uint32"; break;
                case Uint64 : lhs << "Uint64"; break;
                case Bool : lhs << "Bool"; break;
                default : lhs << "Notset"; break;
            }
            return lhs;
        }

        Metric();
        /**
        * The typical metric constructor.
        * @param ns The metric's namespace.
        * @param unit The metric's unit.
        * @param description The metric's description.
        */
        Metric(Namespace &ns, std::string unit,
                std::string description);

        /**
        * Constructor same as above but takes lvalue of "Namespace"
        */
        Metric(Namespace &&ns, std::string unit,
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
        const Namespace& ns() const;

        /**
        * set_ns sets the namespace of the metric in its `rpc::Metric` ptr.
        * It also invalidates the memoization cache of the namespace if it is
        * present.
        * @see memo_ns
        */
        void set_ns(Namespace &ns);

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

        /**
        * set_diagnostic_config is used to apply generated config to specific metric.
        */
        void set_diagnostic_config(const Config& cfg);

        DataType data_type() const;

        /**
        * set_data sets the metric instances data in the underlying rpc::Metric
        * pointer.
        */
        void set_data(int32_t data);
        void set_data(int64_t data);
        void set_data(uint32_t data);
        void set_data(uint64_t data);
        void set_data(float data);
        void set_data(double data);
        void set_data(bool data);
        void set_data(const std::string& data);

        /**
        * Retrieve this metric's datapoint
        */
        int32_t get_int_data() const;
        int64_t get_int64_data() const;
        uint32_t get_uint32_data() const;
        uint64_t get_uint64_data() const;
        float get_float32_data() const;
        double get_float64_data() const;
        bool get_bool_data() const;
        const std::string& get_string_data() const;
        Config get_config() const;
        const rpc::Metric* get_rpc_metric_ptr() const;

        private:
        rpc::Metric* rpc_metric_ptr;
        Config config;

        void inline set_ts(std::chrono::system_clock::time_point tp);
        void inline set_last_advert_tm(std::chrono::system_clock::time_point tp);

        // memoized members
        mutable Namespace memo_ns;
        mutable std::map<std::string, std::string> memo_tags;

        bool delete_metric_ptr;
        DataType type;
    };

}   // namespace Plugin
