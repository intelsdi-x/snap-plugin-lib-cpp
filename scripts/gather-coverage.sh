#!/bin/bash
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

## need to copy data .gcda files close to library source files
pushd ../src/snap; for f in ./.libs/libsnap_la-*.gc*; do cp $f $(echo $(basename $f)|sed 's/libsnap_la-//'); done; popd
pushd ../src/snap/proxy; for f in ./.libs/libsnap_la-*.gc*; do cp $f $(echo $(basename $f)|sed 's/libsnap_la-//'); done; popd
## process coverage data
pushd ..
gcov -n $(find test/ \( -iname '*.o' -or -iname '*.h' \) )  $(find src/ \( -iname '*.o' -or -iname '*.h' \) ) |grep -ve "^\[ \t\]*$"|tr '\n%' '@\n' > test/cov_raw_report.o
for f in $(cd src/snap; ls -1 *.cc |grep -v *.pb) $(cd src/snap/proxy; ls -1 *.cc); do cat test/cov_raw_report.o |awk -F"[':]" '{print $2":"$4}' |awk -F/ '{print $NF}'|grep -v '/usr' |grep $f; done |grep -v grpc |grep -v pb |sort
popd

