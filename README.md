<!--
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
-->

## Snap Plugin Library for C++

This is a library for writing plugins in C++ for the [Snap telemetry framework](https://github.com/intelsdi-x/snap).

Snap has three different plugin types. For instructions on how to write a plugin check out the following links to example plugins:
* [collector](examples/collector/README.md),
* [processor](examples/processor/README.md),
* [publisher](examples/publisher/README.md).

Before writing a Snap plugin:

* See if one already exists in the [Plugin Catalog](https://github.com/intelsdi-x/snap/blob/master/docs/PLUGIN_CATALOG.md)
* See if someone mentioned it in the [plugin wishlist](https://github.com/intelsdi-x/snap/labels/plugin-wishlist)

If you do decide to write a plugin, open a new issue following the plugin [wishlist guidelines](https://github.com/intelsdi-x/snap/blob/master/docs/PLUGIN_CATALOG.md#wish-list) and let us know you are working on one!

## Brief Overview of Snap Architecture

Snap is an open and modular telemetry framework designed to simplify the collection, processing and publishing of data through a single HTTP based API. Plugins provide the functionality of collection, processing and publishing and can be loaded/unloaded, upgraded and swapped without requiring a restart of the Snap daemon.

A Snap plugin is a program that responds to a set of well defined [gRPC](http://www.grpc.io/) services with parameters and returns types specified as protocol buffer messages (see [plugin.proto](https://github.com/intelsdi-x/snap/blob/master/control/plugin/rpc/plugin.proto)). The Snap daemon handshakes with the plugin over stdout and then communicates over gRPC.

## Snap Plugin C++ Library Examples
You will find [example plugins](examples) that cover the basics for writing collector, processor, and publisher plugins in the examples folder.

## Building libsnap:

Dependencies:
* autoconf
* automake
* curl
* g++
* grpc++ (see below)
* libtool
* make

### Building GRPC from source

Plugin currently depends on GRPC 1.0.x, so if your OS's package repository contains another version, it won't be compatible.
First, you need to install all dependencies except GRPC, then build GRPC 1.0.x from source.

#### To install GRPC 1.0.1:
```bash
git clone https://github.com/grpc/grpc
cd grpc
git checkout tags/v1.0.1
git submodule update --init
make
[sudo] make install
```

#### To install protobuf (Google Protocol Buffers):
Snap plugin library for C++ depends also on protobuf library. As it is already in GRPC dependencies, you can install it like:
```bash
cd ./third_party/protobuf
[sudo] make install
```

### Once the above dependencies have been resolved:

```sh
$ ./autogen.sh
$ ./configure
$ make
$ [sudo] make install
```

`autotools` installs `libsnap` into `/usr/local/lib`, which not all linkers use when searching for shared objects.  Using the `--prefix=/usr` switch when running the `configure` script will place the resulting libraries into `/usr/lib`, for example.

To clean up and rebuild use:
```sh
$ make clean
$ git clean -df  # warning! This deletes all dirs and files not checked in.  Be sure to check in any new files before running `git clean`.
```
