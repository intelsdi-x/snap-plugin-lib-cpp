#! /bin/bash
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

rm snap-plugin-collector-rando
sudo rm /usr/lib/libsnap.so
find src/libsnap -name "*.cc" | xargs -I{} clang++ --std=c++11 -Wall -fPIC -c -I include -I include/snap/rpc -I ../json/src {}
clang++ -shared -lprotobuf -lgrpc++ -o libsnap.so *.o
rm *.o
sudo mv libsnap.so /usr/lib
clang++ --std=c++11 -I examples/rando/include -I include -l snap -o snap-plugin-collector-rando examples/rando/src/rando.cc
