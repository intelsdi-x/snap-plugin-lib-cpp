Snap Plugin Library for C++
===========================

_Note: This repository is a work in progress and is not, by any means, ready to be used._

See the [RPC Type, Content Types, and Client Libraries RFC](https://github.com/intelsdi-x/snap/issues/1038) on Snap as a reference for the plans for this repository.

## Development Notes

Things you need to compile grpc++:
* autoconf
* automake
* gettext
* libtool

After that:
```sh
 $ git clone https://github.com/grpc/grpc.git
 $ cd grpc
 $ git submodule update --init
 $ make
 $ [sudo] make install
 ```
Once grpc++ is installed you need these two commands:
```sh
$ protoc \
  --grpc_out=`pwd` \
  --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` \
  --proto_path=${GOPATH}/src/github.com/intelsdi-x/snap/control/plugin/rpc \
  ${GOPATH}/src/github.com/intelsdi-x/snap/control/plugin/rpc/plugin.proto
$ protoc \
  --cpp_out=`pwd` \
  --proto_path=${GOPATH}/src/github.com/intelsdi-x/snap/control/plugin/rpc \
  ${GOPATH}/src/github.com/intelsdi-x/snap/control/plugin/rpc/plugin.proto
```
Compiling and installing libsnap:
```sh
$ find src/libsnap -name "*.cc" | xargs -I{} clang++ --std=c++11 -Wall -fPIC -c -I include -I include/snap/rpc {} && clang++ -shared -lprotobuf -lgrpc++ -o libsnap.so *.o && rm *.o && sudo mv libsnap.so /usr/lib
```

To build libsnap, it must be linked against libprotobuf and libgrpc.  Then plugins are linked against libsnap 

```sh
$ clang++ --std=c++11 -I examples/rando/include -I include -l snap -o snap-collector-rando examples/rando/src/rando.cc
```
