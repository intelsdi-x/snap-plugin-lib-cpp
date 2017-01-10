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

Snap Plugin Library for C++
===========================

_Note: This repository is a work in progress and is not, by any means, ready to be used._

See the [RPC Type, Content Types, and Client Libraries RFC](https://github.com/intelsdi-x/snap/issues/1038) on Snap as a reference for the plans for this repository.

## Building libsnap:

Dependencies:
* autoconf
* automake
* libtool
* grpc++

Once the above dependencies have been resolved:

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
