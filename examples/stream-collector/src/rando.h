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
#include <vector>
#include <iostream>

#include <snap/config.h>
#include <snap/metric.h>
#include <snap/plugin.h>
#include <snap/flags.h>


class Rando final : public Plugin::StreamCollectorInterface {   
public:
    const Plugin::ConfigPolicy get_config_policy();
    std::vector<Plugin::Metric> get_metric_types(Plugin::Config cfg);

    void stream_metrics();

    void stream_it();

    void drain_metrics();
    
    std::vector<Plugin::Metric> put_metrics_out() { return _metrics_out; }
    std::string put_err_msg() { return _err_msg; }
    
    void get_metrics_in(std::vector<Plugin::Metric> &metsIn) {
        _metrics_in.clear();
        std::copy(metsIn.begin(), metsIn.end(), std::back_inserter(_metrics_in));
        _get_mets = true;
    }

    bool put_mets() { return _put_mets; }
    void set_put_mets(const bool &putMets) { _put_mets = putMets; }
    bool put_err() { return _put_err; }
    void set_put_err(const bool &putErr) { _put_err = putErr; }

private:
    std::vector<Plugin::Metric> _metrics_out;
    std::vector<Plugin::Metric> _metrics_in;
    std::string _err_msg;

    bool _put_mets = false, _put_err = false, _get_mets = false; 
};
