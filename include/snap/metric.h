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

#include <ctime>
#include <map>
#include <string>

namespace Metric {

// TODO (danielscottt): use a union for data here.
class Metric {

  public:
    void setData(int data);

  private:
    Namespace ns;
    int data;
    time_t lastAdvertisedTime;
    time_t timestamp;
    std::map<std::string, std::string> tags;
    std::string description;
    std::string unit;
}

class Namespace : private std::vector<std::string> {
  public:
    Namespace();
    ~Namespace();
}

} // namespace Metric
