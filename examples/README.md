<!--
http://www.apache.org/licenses/LICENSE-2.0.txt


Copyright 2017 Intel Corporation

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

## Snap Plugin C++ Library Examples
Here you will find example plugins that cover the basics for writing collector, processor and publisher plugins.

## Build Plugins & Use with Snap

To get these example collector, processor, and publisher plugins to build properly and work with Snap you will need to install dependencies mentioned in [repository README](../README.md#building-libsnap).  You should also add snaptel and snapteld in your $PATH. 
To test these plugins with Snap, you will need to have [Snap](https://github.com/intelsdi-x/snap) installed, check out these docs for [Snap setup details](https://github.com/intelsdi-x/snap/blob/master/docs/BUILD_AND_TEST.md#getting-started).

### 1. Get the plugin library repo:
`git clone github.com/intelsdi-x/snap-plugin-lib-cpp.git`

### 2. Build and install the library
Refer to [repository README](../README.md#building-libsnap) for installation instructions.

### 3. Build the collector, processor, and/or publisher plugins in the examples folder.
Enter the examples directory and execute `make`:

```sh
$ cd examples
$ make
```

### 4. Launch example task in `examples` directory:

```sh
$ cd examples
$ ./run.sh
```

### 5. Verify that the tasks are running:

```sh
$ snaptel task list

ID 					 NAME 					 STATE 		 HIT 	 MISS 	 FAIL 	 CREATED 		 LAST FAILURE
7d614c46-e34b-4d6f-9de7-6731cf63b6ad 	 Task-7d614c46-e34b-4d6f-9de7-6731cf63b6ad 	 Running 	 191 	 0 	 0 	 2:06PM 1-10-2017
```
