#! /bin/bash

rm snap-plugin-collector-rando
sudo rm /usr/lib/libsnap.so
find src/libsnap -name "*.cc" | xargs -I{} clang++ --std=c++11 -Wall -fPIC -c -I include -I include/snap/rpc -I ../json/src {}
clang++ -shared -lprotobuf -lgrpc++ -o libsnap.so *.o
rm *.o
sudo mv libsnap.so /usr/lib
clang++ --std=c++11 -I examples/rando/include -I include -l snap -o snap-plugin-collector-rando examples/rando/src/rando.cc
