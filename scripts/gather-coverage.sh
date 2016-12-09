#!/bin/bash

## need to copy data .gcda files close to library source files
pushd ../src/snap; for f in ./.libs/libsnap_la-*.gc*; do cp $f $(echo $(basename $f)|sed 's/libsnap_la-//'); done; popd
pushd ../src/snap/proxy; for f in ./.libs/libsnap_la-*.gc*; do cp $f $(echo $(basename $f)|sed 's/libsnap_la-//'); done; popd
## process coverage data
pushd ..
gcov -n $(find test/ \( -iname '*.o' -or -iname '*.h' \) )  $(find src/ \( -iname '*.o' -or -iname '*.h' \) ) |grep -ve "^\[ \t\]*$"|tr '\n%' '@\n' |tee test/cov_raw_report.o
for f in $(cd src/snap; ls -1 *.cc |grep -v *.pb) $(cd src/snap/proxy; ls -1 *.cc); do cat test/cov_raw_report.o |awk -F"[':]" '{print $2":"$4}' |awk -F/ '{print $NF}'|grep -v '/usr' |grep $f; done |grep -v grpc |grep -v pb |sort
popd

